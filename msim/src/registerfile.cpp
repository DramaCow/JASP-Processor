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

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile)
{
  for (int i = 0; i < NUM_REGISTERS; ++i)
  {
    os << std::setfill('0') << std::setw(10) << std::dec << regfile.gpr[i] << '_' << (regfile.v[i] ? '1' : '0') << ' ';
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
