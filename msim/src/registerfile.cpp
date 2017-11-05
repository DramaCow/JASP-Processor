#include "registerfile.hpp"
#include <cstring>
#include <iomanip>

RegisterFile::RegisterFile()
{
  std::fill_n(v, NUM_REGISTERS, true);
}

std::tuple<uint32_t, uint32_t> RegisterFile::foo(uint32_t addr1, uint32_t addr2, uint32_t daddr, uint32_t data, bool we)
{
  if (we)
  {
    gpr[daddr] = data;
  }
  return std::make_tuple(gpr[addr1], gpr[addr2]);
}

std::tuple<int, bool> RegisterFile::read(int addr)
{
  if (!this->v[addr])
  {
    return std::make_tuple(addr, false);
  }
  return std::make_tuple(this->gpr[addr], true);
}

void RegisterFile::reset(int addr)
{
  this->v[addr] = false;
}

void RegisterFile::write(int addr, int data)
{
  this->gpr[addr] = data;
  this->v[addr] = true;
}

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    //os << std::setfill('0') << std::setw(10);
    os << std::dec << regfile.gpr[i];
    //os << '_' << (regfile.v[i] ? '1' : '0');
    os << ' ';
    if (i % 8 == 7 && i < NUM_REGISTERS - 1)
    {
      os << '\n';
    }
  }
  return os;
}

RegisterFile& RegisterFile::operator=(const RegisterFile& regfile)
{
  memcpy(this->gpr, regfile.gpr, NUM_REGISTERS*sizeof(uint32_t));
  return *this;
}
