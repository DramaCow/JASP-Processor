#include "mu.hpp"
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

MU::MU(DCache &dcache) :
  dcache(dcache)
{
}

MU& MU::operator=(const MU& mu)
{
  return *this;
}

std::ostream& operator<<(std::ostream& os, const MU& mu)
{
  return os;
}
