#include "rob.hpp"

std::tuple<int, bool> ROB::read(int addr)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  return std::make_tuple(this->done[a] ? this->val[a] : this->reg[a], this->done[a]);
}

int ROB::alloc(int r)
{
  // alloc the rob entry
  this->reg[this->head] = r;
  this->val[this->head] = 0;
  this->done[this->head] = false;

  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int e = this->head + NUM_REGISTERS;

  // move the head (w/ wrap around)
  this->head = (this->head + 1) % NUM_ROB_ENTRIES;

  return e;
}

std::ostream& operator<<(std::ostream& os, const ROB& rob)
{
  os << "    head = " << rob.head+NUM_REGISTERS << '\n';
  os << "    tail = " << rob.tail+NUM_REGISTERS << '\n';
  for (int i = 0; i < NUM_ROB_ENTRIES; ++i)
  {
    os << "      " << i+NUM_REGISTERS << ": " << rob.reg[i] << ", " << rob.val[i] << ", " << rob.done[i] << '\n';
  }
  return os;
}
