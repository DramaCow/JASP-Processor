#include "rob.hpp"

int ROB::alloc(int r)
{
  return 0;
}

std::ostream& operator<<(std::ostream& os, const ROB& rob)
{
  os << "    head = " << rob.head << '\n';
  os << "    tail = " << rob.tail << "\n    ";
  for (int i = 0; i < NUM_ROB_ENTRIES; ++i)
  {
    os << '(' << rob.reg[i] << ", " << rob.val[i] << ", " << rob.done[i] << "), ";
  }
  return os;
}
