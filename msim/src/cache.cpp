#include "cache.hpp"

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

DCache::DCache(int size) :
  size(size)
{
  this->data = new int[size]{ 0 };
}

DCache::~DCache()
{
  delete[] data;
}

void DCache::initialise(int *data, int size)
{
  delete[] this->data;
  this->data = data;
  this->size = size;
}

int& DCache::operator[](int i)
{
  return this->data[i];
}

std::ostream& operator<<(std::ostream& os, const DCache& dcache)
{
  os << "    ";
  for (int i = 0; i < dcache.size; ++i)
  {
    os << dcache.data[i] << ' ';
    if (i % 8 == 7 && i != dcache.size-1)
    {
      os << "\n    ";
    }
  }
  os << '\n';
  return os;
}
