#include "lsu.hpp"
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

LSU::LSU(DCache &dcache) :
  dcache(dcache)
{
}

void LSU::dispatch(std::string opcode, int seq, int base, int offset, int val, int tail)
{
  if (opcode == "nop")
  {
    return;
  }

  Entry entry;
  if (opcode == "lw")
  {
    entry.type = LSU::Entry::LOAD;
  }
  else if (opcode == "sw")
  {
    entry.type = LSU::Entry::STORE;
  }
  entry.seq = seq;
  entry.addr = base + offset;
  entry.val = val;

  std::size_t i;
  for (i = 0; i < this->entries.size(); ++i)
  {
    int s1 = entry.seq + (entry.seq < tail ? NUM_ROB_ENTRIES : 0);
    int s2 = this->entries[i].seq + (this->entries[i].seq < tail ? NUM_ROB_ENTRIES : 0);

    if (s1 > s2)
    {
      break;
    }
  }

  // if inserting a load:
  // - check all above instructions until last store
  // - if store accesses same address as load, forward its value
  // - note: if there exists and store between the forwarded store and load still in the rs
  //         then the assumption is that this store does not access the same address
  //         as the load, and that the forwarded store contains the most up to date value
  //         for that memory address. This may not be the case, in which inserting the inbetween
  //         store into the LSU will automatically update the load val before it is executed
  //         or the mis-speculation is detected during the commit pipeline stage and corrections are made.
  if (entry.type == LSU::Entry::LOAD)
  {
    for (int j = i-1; j >= 0; --j)
    {
      if (this->entries[j].type == LSU::Entry::STORE)
      {
        if (entry.addr == this->entries[j].addr)
        {
          entry.val = this->entries[j].val;
          entry.fwd = true;
        }
        break;
      }
    }
  }
  // if inserting a store:
  // - check all subsequent instructions until reached a store that modifies the same memory address
  // - each load instruction encountered, forward the store value to the load
  else if (entry.type == LSU::Entry::STORE)
  {
    for (std::size_t j = i+1; j < this->entries.size(); ++j)
    {
      if (this->entries[j].type == LSU::Entry::LOAD && this->entries[j].addr == entry.addr)
      {
        this->entries[j].val = entry.val;
        this->entries[j].fwd = true;
      }
      else if (this->entries[j].type == LSU::Entry::STORE && this->entries[j].addr == entry.addr)
      {
        break;
      }
    }
  }

  this->entries.insert(std::begin(this->entries) + i, entry);
}

void LSU::execute(LSU& n_lsu)
{
  n_lsu.duration = this->duration-1 > 0 ? this->duration-1 : 0;

  if (this->duration == 0)
  {
    if (n_lsu.entries.size() > 0)
    {
      n_lsu.next = n_lsu.entries[0];
      n_lsu.entries.erase(std::begin(n_lsu.entries));

      if (n_lsu.next.type == LSU::Entry::LOAD)
      {
        // if val has been fowarded from some previous store
        if (n_lsu.next.fwd)
        {
          n_lsu.duration = 0;
          n_lsu.writeback = true;
        }
        // else have to take our time grabbing from memory
        else
        {
          n_lsu.next.val = dcache[n_lsu.next.addr];
          n_lsu.duration = 1;
          n_lsu.writeback = true;
        }
      }
      else if (n_lsu.next.type == LSU::Entry::STORE)
      {
        // store is handled at commit
        n_lsu.duration = 0;
        n_lsu.writeback = true;
      }
    }
    else
    {
      n_lsu.writeback = false;
    }
  }
}

bool LSU::isAvailable()
{
  return this->entries.size() < NUM_LSQ_ENTRIES;
}

LSU& LSU::operator=(const LSU& lsu)
{
  this->entries = lsu.entries;
  this->duration = lsu.duration;
  this->next = lsu.next;
  this->writeback = lsu.writeback;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const LSU& lsu)
{
  os << "    " << std::to_string(lsu.entries.size()) << std::endl;;
  os << "    type  seq   addr  fwd   \n";
  if (lsu.next.type != LSU::Entry::NA)
  {
    os << "    ------------------------\n";
    os << "    ";
    os << SPACE(lsu.next.type == LSU::Entry::LOAD ? "L" : "S")
       << SPACE(std::to_string(lsu.next.seq))
       << SPACE(std::to_string(lsu.next.addr))
       << SPACE(std::to_string(lsu.next.fwd))
       << (lsu.writeback ? "  writeback" : "")
       << '\n';
  }
  os << "    ------------------------\n";
  for (std::size_t i = 0; i < lsu.entries.size(); ++i)
  {
    os << "    ";
    os << SPACE(lsu.entries[i].type == LSU::Entry::LOAD ? "L" : "S")
       << SPACE(std::to_string(lsu.entries[i].seq))
       << SPACE(std::to_string(lsu.entries[i].addr))
       << SPACE(std::to_string(lsu.entries[i].fwd))
       << '\n';
  }
  return os;
}
