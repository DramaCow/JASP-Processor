#include "rat.hpp"
#include <cstring>

RAT::RAT()
{
  this->reset();
}

int RAT::read(int r)
{
  return this->table[r];
}

void RAT::write(RAT &n_rat, int r, int a)
{
  n_rat.table[r] = a;
}

void RAT::reset()
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    this->table[i] = i;
  }
}

RAT& RAT::operator=(const RAT& rat)
{
  memcpy(this->table, rat.table, NUM_REGISTERS*sizeof(int));
  return *this;
}

std::ostream& operator<<(std::ostream& os, const RAT& rat)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    os << rat.table[i] << ' ';
  }
  return os;
}
