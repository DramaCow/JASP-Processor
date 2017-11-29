#include "rob.hpp"
#include <cstring>

int ROB::push(ROB &n_rob, std::string opcode, int r, int target)
{
  ROBEntry::Type type = ROBEntry::DN;
  if (Instruction::isArth(opcode) || opcode == "ld")
  {
    type = ROBEntry::WB;
  }
  else if(Instruction::isBrch(opcode))
  {
    type = ROBEntry::BR;
  }

  // alloc the rob entry
  n_rob.entries[this->head].type = type;
  n_rob.entries[this->head].reg = r;
  n_rob.entries[this->head].val = 0;
  n_rob.entries[this->head].target = target;
  n_rob.entries[this->head].done = false;

  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int e = this->head + NUM_REGISTERS;

  // move the head (w/ wrap around)
  n_rob.head = (this->head + 1) % NUM_ROB_ENTRIES;

  return e;
}

std::vector<std::tuple<int,ROB::ROBEntry>> ROB::pop(ROB &n_rob)
{
  std::vector<std::tuple<int,ROB::ROBEntry>> commits;

  int c;
  for (c = 0; c < 4; ++c) // max commits per cycle is 4
  {
    int tail = (this->tail + c) % NUM_REGISTERS;
    if (!this->entries[tail].done || this->head == tail)
    {
      break;
    }
    commits.push_back(std::make_tuple(tail, this->entries[tail]));
  }

  n_rob.tail = (this->tail + c) % NUM_REGISTERS;

  return commits;
}

std::tuple<int, bool> ROB::read(int addr)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  return std::make_tuple(this->entries[a].done ? this->entries[a].val : addr, this->entries[a].done);
}

void ROB::write(int addr, int val)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  this->entries[a].val = val;
  this->entries[a].done = true;
}

ROB& ROB::operator=(const ROB& rob)
{
  this->head = rob.head;
  this->tail = rob.tail;
  memcpy(this->entries, rob.entries, NUM_ROB_ENTRIES*sizeof(ROBEntry));
  return *this;
}

std::ostream& operator<<(std::ostream& os, const ROB& rob)
{
  for (int i = 0; i < NUM_ROB_ENTRIES; ++i)
  {
    os << "    " << i+NUM_REGISTERS << ": ";
    switch (rob.entries[i].type)
    {
      case ROB::ROBEntry::DN: os << "DN"; break;
      case ROB::ROBEntry::WB: os << "WB"; break;
      case ROB::ROBEntry::BR: os << "BR"; break;
      default: os << "??"; break;
    }
    os << ", " << rob.entries[i].reg << ", " << rob.entries[i].val << ", " << rob.entries[i].target << ", " << rob.entries[i].done;
    if (rob.head == i)
    {
      os << " <-h-";
    }
    if (rob.tail == i)
    {
      os << " <-t-";
    }
    os << '\n';
  }
  return os;
}
