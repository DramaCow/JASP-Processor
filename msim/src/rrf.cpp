#include "rrf.hpp"
#include <cstring>
#include <iomanip>

RRF::RRF()
{
  std::fill_n(v, NUM_REGISTERS, true);
}

std::tuple<uint32_t, uint32_t> RRF::foo(uint32_t addr1, uint32_t addr2, uint32_t daddr, uint32_t data, bool we)
{
  if (we)
  {
    gpr[daddr] = data;
  }
  return std::make_tuple(gpr[addr1], gpr[addr2]);
}

std::tuple<int, bool> RRF::read(int addr)
{
  if (!this->v[addr])
  {
    return std::make_tuple(addr, false);
  }
  return std::make_tuple(this->gpr[addr], true);
}

void RRF::reset(int addr)
{
  this->v[addr] = false;
}

void RRF::write(int addr, int data)
{
  this->gpr[addr] = data;
  this->v[addr] = true;
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
