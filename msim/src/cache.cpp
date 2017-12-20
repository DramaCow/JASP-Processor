#include "cache.hpp"
#include <cstring>

ICache::ICache(Instruction *program, int size) :
  program(program),
  size(size)
{
}

Instruction ICache::operator[](int i)
{
  if (0 <= i && i < size)
  {
    return this->program[i];
  }
  return Instruction();
}

ICache::~ICache()
{
  delete[] program;
}

std::ostream& operator<<(std::ostream& os, const ICache& icache)
{
  for (int i = 0; i < icache.size; ++i)
  {
    os << icache.program[i] << '\n';
  }
  return os;
}
