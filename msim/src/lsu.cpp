#include "lsu.hpp"
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

void LSU::insert(std::string opcode, int seq, int o1, int o2, int tail)
{
  Entry entry;
  if (opcode == "lw")
  {
    entry.type == LSU::Entry::LOAD;
  }
  else if (opcode == "sw")
  {
    entry.type == LSU::Entry::STORE;
  }
  entry.seq = seq;
  entry.addr = o1 + o2;

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
      }
      else if (this->entries[j].type == LSU::Entry::STORE && this->entries[j].addr == entry.addr)
      {
        break;
      }
    }
  }

  this->entries.insert(std::begin(this->entries) + i, entry);
}

std::ostream& operator<<(std::ostream& os, const LSU& lsu)
{
  os << "    type  seq   addr  \n";
  os << "    ------------------\n";
  for (std::size_t i = 0; i < lsu.entries.size(); ++i)
  {
    os << "    ";
    os << SPACE(lsu.entries[i].type == LSU::Entry::LOAD ? "L" : "S")
       << SPACE(std::to_string(lsu.entries[i].seq))
       << SPACE(std::to_string(lsu.entries[i].addr))
       << '\n';
  }
  return os;
}
