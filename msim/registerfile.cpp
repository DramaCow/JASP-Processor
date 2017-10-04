#include "registerfile.hpp"
#include <iomanip>

RegisterFile::RegisterFile()
{
}

std::tuple<uint32_t, uint32_t> RegisterFile::foo(uint32_t addr1, uint32_t addr2, uint32_t daddr, uint32_t data, bool write)
{
  if (write)
  {
    gpr[daddr] = data;
  }
  return std::make_tuple(gpr[addr1], gpr[addr2]);
}

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile)
{
  for (int i = 0; i < 32; ++i)
  {
    os << std::setfill('0') << std::setw(8) << std::hex << regfile.gpr[i] << ' ';
    if (i % 8 == 7)
    {
      os << '\n';
    }
  }
  return os;
}
