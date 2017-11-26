#include "rat.hpp"

RAT::RAT()
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    this->table[i] = i;
  }
}

int RAT::read(int r)
{
  return this->table[r];
}

int RAT::alloc(int r)
{
  return this->table[r]; // TODO
}

std::ostream& operator<<(std::ostream& os, const RAT& rat)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    os << rat.table[i] << ' ';
  }
  return os;
}
