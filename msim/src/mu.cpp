#include "mu.hpp"
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

MU::MU(DCache &dcache, SAC &l1cache, SAC &l2cache, MEM &mem) :
  dcache(dcache),
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
    if (this->shelf.fwd)
    {
      this->result = this->shelf.w;
      this->duration = 0;
    }
    else
    {
      this->result = this->dcache[this->shelf.addr];
      this->duration = 1;
    }
    this->writeback = true;
  }
  else if (this->shelf.type == LSQ::Shelf::STORE)
  {
    // NOTE: this writes immediately but pretends to take a while, so
    //       whilst the number of cycles take to perform operation is
    //       somewhat realistic, the memory displayed is ahead for
    //       a couple of cycles. This is really a problem though.
    this->dcache[this->shelf.addr] = this->shelf.w;
    this->duration = 1;
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

void MU::reset()
{
  LSQ::Shelf shelf;
  this->shelf = shelf;
  this->duration = 0;
  this->writeback = false;
}

void MU::l1_cachemiss()
{
}

void MU::l2_cachemiss()
{
}

SAC::Line * MU::getline(int baddr)
{
  SAC::Line *line = nullptr;

  line = this->l1cache.access(baddr);
  if (line == nullptr)
  {
    line = this->l2cache.access(baddr);
    if (line == nullptr)
    {
      line = new SAC::Line(mem.getblock(baddr), baddr);
      
    }
  }

  return line;
}

MU& MU::operator=(const MU& mu)
{
  this->shelf = mu.shelf;
  this->result = mu.result;
  this->duration = mu.duration;
  this->writeback = mu.writeback;
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
