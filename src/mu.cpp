#include "mu.hpp"
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

MU::MU(SAC &l1cache, SAC &l2cache, MEM &mem) :
  l1cache(l1cache),
  l2cache(l2cache),
  mem(mem)
{
}

bool MU::isReady()
{
  return true;
}

void MU::dispatch(LSQ::Shelf shelf)
{
  this->shelf = shelf;

  if (this->shelf.type == LSQ::Shelf::LOAD)
  {
    this->duration = 0;
    if (this->shelf.fwd)
    {
      this->result = this->shelf.w;
    }
    else
    {
      this->result = this->load(this->shelf.addr);
      this->duration += BASE_MU_COST;
    }
    this->writeback = true;
  }
  else if (this->shelf.type == LSQ::Shelf::STORE)
  {
    // NOTE: this writes immediately but pretends to take a while, so
    //       whilst the number of cycles take to perform operation is
    //       somewhat realistic, the memory displayed is ahead for
    //       a couple of cycles. This is really a problem though.
    this->duration = 0;
    this->store(this->shelf.addr, this->shelf.w);
    this->duration += BASE_MU_COST;
    this->writeback = true;
  }
  else
  {
    this->duration = 0;
    this->writeback = false;
  }
}

// applied to the current state, updates the next state
void MU::execute(MU& n_mu)
{
  n_mu.duration = this->duration-1 > 0 ? this->duration-1 : 0;
}

#if CACHELEVELS != 1
void MU::complete_writethrough()
{
  // first pass: writethrough l2cache (dirty entries will be older than l1cache)
  for (std::size_t i = 0; i < this->l2cache.lines.size(); ++i)
  {
    if (this->l2cache.lines[i].valid && this->l2cache.lines[i].dirty)
    {
      this->bMEM(&this->l2cache.lines[i]);
    }
  }

  // second pass: writethrough l1cache
  for (std::size_t i = 0; i < this->l1cache.lines.size(); ++i)
  {
    if (this->l1cache.lines[i].valid && this->l1cache.lines[i].dirty)
    {
      this->bMEM(&this->l1cache.lines[i]); // fuck it; go directly to memory
    }
  }
}
#else
void MU::complete_writethrough()
{
  for (std::size_t i = 0; i < this->l1cache.lines.size(); ++i)
  {
    if (this->l1cache.lines[i].valid && this->l1cache.lines[i].dirty)
    {
      this->bMEM(&this->l1cache.lines[i]);
    }
  }
}
#endif

void MU::reset()
{
  LSQ::Shelf shelf;
  this->shelf = shelf;
  this->duration = 0;
  this->writeback = false;
}

int MU::load(int addr)
{
  int offset = addr % BLOCKSIZE; // offset in block
  int baddr  = addr / BLOCKSIZE; // address of block in memory

  const SAC::Line * line = this->rL1(baddr);
  int val = line->data[offset];
  return val;
}

void MU::store(int addr, int val)
{
  int offset = addr % BLOCKSIZE; // offset in block
  int baddr  = addr / BLOCKSIZE; // address of block in memory

  SAC::Line * line = this->rL1(baddr);
  line->data[offset] = val;
  line->dirty = true;
}

#if CACHELEVELS != 1
SAC::Line * MU::rL1(int baddr)
{
  this->duration += L1_ACCESS_COST;
  this->l1_accesses++;

  SAC::Line *line = this->l1cache.access(baddr);
  if (line == nullptr) // miss
  {
    return this->rL2(baddr);
  }

#if CACHELEVELS == 0
  // just being lazy
  this->mem_accesses += this->l1_accesses;
  this->mem_accesses += this->l2_accesses;
  this->l1_accesses = this->l2_accesses = 0;
#endif

  return line; // hit
}
SAC::Line * MU::rL2(int baddr)
{
  this->duration += L2_ACCESS_COST;
  this->l2_accesses++;

  SAC::Line *line = this->l2cache.access(baddr);
  if (line == nullptr) // miss
  {
    return this->rMEM(baddr);
  }
  return this->wL1(line); // hit
}
SAC::Line * MU::rMEM(int baddr)
{
  this->duration += MEM_ACCESS_COST;
  this->mem_accesses++;

  return this->wL2(new SAC::Line(this->mem.getblock(baddr), baddr));
}
SAC::Line * MU::wL1(SAC::Line *line)
{
  this->duration += L1_ACCESS_COST;
  this->l1_accesses++;

  SAC::Line *newLine, *replaceLine;
  std::tie(newLine, replaceLine) = this->l1cache.stash(line->baddr, line);
  if (replaceLine != nullptr) // dirty
  {
    this->bL2(replaceLine);
    delete replaceLine;
  }
  return newLine;
}
SAC::Line * MU::wL2(SAC::Line *line)
{
  this->duration += L2_ACCESS_COST;
  this->l2_accesses++;

  SAC::Line *newLine, *replaceLine;
  std::tie(newLine, replaceLine) = this->l2cache.stash(line->baddr, line);
  delete line; // line is from memory (I know, this line hurts me too...)
  if (replaceLine != nullptr) // dirty
  {
    this->bMEM(replaceLine);
    delete replaceLine;
  }
  return this->wL1(newLine);
}
void MU::bL2(SAC::Line *line)
{
  this->duration += L2_ACCESS_COST;
  this->l2_accesses++;

  SAC::Line *replaceLine;
  std::tie(std::ignore, replaceLine) = this->l2cache.stash(line->baddr, line);
  if (replaceLine != nullptr) // dirty
  {
    this->bMEM(replaceLine);
    delete replaceLine;
  }
}
void MU::bMEM(SAC::Line *line)
{
  this->duration += MEM_ACCESS_COST;
  this->mem_accesses++;

  this->mem.writeblock(line->baddr, line->data);
}
#else
SAC::Line * MU::rL1(int baddr)
{
  this->duration += L1_ACCESS_COST;
  this->l1_accesses++;

  SAC::Line *line = this->l1cache.access(baddr);
  if (line == nullptr) // miss
  {
    return this->rMEM(baddr);
  }
  return line; // hit
}
SAC::Line * MU::rMEM(int baddr)
{
  this->duration += MEM_ACCESS_COST;
  this->mem_accesses++;

  return this->wL1(new SAC::Line(this->mem.getblock(baddr), baddr));
}
SAC::Line * MU::wL1(SAC::Line *line)
{
  this->duration += L1_ACCESS_COST;
  this->l1_accesses++;

  SAC::Line *newLine, *replaceLine;
  std::tie(newLine, replaceLine) = this->l1cache.stash(line->baddr, line);
  delete line; // line is from memory (I know, this line hurts me too...)
  if (replaceLine != nullptr) // dirty
  {
    this->bMEM(replaceLine);
    delete replaceLine;
  }
  return newLine;
}
void MU::bMEM(SAC::Line *line)
{
  this->duration += MEM_ACCESS_COST;
  this->mem_accesses++;

  this->mem.writeblock(line->baddr, line->data);
}
#endif

MU& MU::operator=(const MU& mu)
{
  this->shelf = mu.shelf;
  this->result = mu.result;
  this->duration = mu.duration;
  this->writeback = mu.writeback;

  this->l1_accesses = mu.l1_accesses;
  this->l2_accesses = mu.l2_accesses;
  this->mem_accesses = mu.mem_accesses;

  return *this;
}

std::ostream& operator<<(std::ostream& os, const MU& mu)
{
  if (mu.shelf.type == LSQ::Shelf::NA)
  {
    os << "    --- nop ---\n";
    return os;
  }
  else if (mu.shelf.type == LSQ::Shelf::LOAD)
  {
    if (!mu.shelf.fwd)
    {
      os << "    d" << mu.shelf.d << " <-- [" << mu.shelf.addr << ']';
    }
    else
    {
      os << "    d" << mu.shelf.d << " <-- " << mu.shelf.w << " (fowarded)";
    }
  }
  else if (mu.shelf.type == LSQ::Shelf::STORE)
  {
    os << "    [" << mu.shelf.addr << "] <-- " << mu.shelf.w;
  }

  os << " (" << mu.duration << ")\n";
  if (mu.writeback && mu.duration == 0)
  {
    os << "    ";
    if (mu.shelf.type == LSQ::Shelf::LOAD)
    {
      os << "= " << mu.result << ' ';
    }
    os << "writeback\n";
  }
  
  return os;
}
