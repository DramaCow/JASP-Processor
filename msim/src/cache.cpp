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
  return Instruction("nop");
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
//  for (int i = 0; i < size; ++i)
//  {
//    this->data[i] = i+1;
//  }
  this->data[0] = 5;
  this->data[1] = 1;
  this->data[2] = 3;
  this->data[3] = 7;
  this->data[4] = 8;
  this->data[5] = 2;
  this->data[6] = 4;
  this->data[7] = 6;
}

DCache::DCache(int *data, int size) :
  data(data),
  size(size)
{
}

DCache::~DCache()
{
  delete[] data;
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
