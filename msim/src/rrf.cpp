#include "rrf.hpp"
#include <cstring>
#include <iomanip>

RRF::RRF()
{
  std::fill_n(v, NUM_REGISTERS, true);
}

std::tuple<int, bool> RRF::read(int r)
{
  if (!this->v[r])
  {
    return std::make_tuple(r, false);
  }
  return std::make_tuple(this->gpr[r], true);
}

void RRF::reset(int r)
{
  this->v[r] = false;
}

void RRF::write(int r, int data)
{
  this->gpr[r] = data;
  this->v[r] = true;
}

bool RRF::operator==(const RRF& rrf)
{
  bool equal = true;
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    equal &= this->gpr[i] == rrf.gpr[i];
  }
  return equal;
}

RRF& RRF::operator=(const RRF& rrf)
{
  memcpy(this->gpr, rrf.gpr, NUM_REGISTERS*sizeof(uint32_t));
  memcpy(this->v, rrf.v, NUM_REGISTERS*sizeof(bool));
  return *this;
}

std::ostream& operator<<(std::ostream& os, const RRF& rrf)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    //os << std::setfill('0') << std::setw(10);
    os << std::dec << rrf.gpr[i];
    //os << '_' << (rrf.v[i] ? '1' : '0');
    os << ' ';
    if (i % 8 == 7 && i < NUM_REGISTERS - 1)
    {
      os << '\n';
    }
  }
  return os;
}
