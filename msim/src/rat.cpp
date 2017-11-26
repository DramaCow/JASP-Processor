#include "rat.hpp"

RAT::RAT(ROB rob, RRF rrf) :
  rob(rob),
  rrf(rrf)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    this->table[i] = i;
  }
}

int RAT::read(int a)
{
  return this->table[a];
}

int RAT::alloc(int a)
{
  return this->table[a]; // TODO
}

std::ostream& operator<<(std::ostream& os, const RAT& rat)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    os << rat.table[i] << ' ';
  }
  return os;
}
