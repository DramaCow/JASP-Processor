#include "mem.hpp"
#include <cmath>

MEM::MEM(int *data, int size) :
  blocks((int)std::ceil((double)size/BLOCKSIZE))
{
  int numBlocks = blocks.size();
  for (int j = 0; j < numBlocks; ++j)
  {
    for (int i = 0; i < std::min(BLOCKSIZE,size-j*BLOCKSIZE); ++i)
    {
      this->blocks[j][i] = data[j*BLOCKSIZE + i];
    }
  }
}

std::array<int,BLOCKSIZE> MEM::getblock(int a)
{
  return this->blocks[a];
}

void MEM::writeblock(int a, std::array<int,BLOCKSIZE> block)
{
  this->blocks[a] = block;
}

std::ostream& operator<<(std::ostream& os, const MEM& mem)
{
  for (std::size_t j = 0; j < mem.blocks.size(); ++j)
  {
    for (std::size_t i = 0; i < BLOCKSIZE; ++i)
    {
      os << mem.blocks[j][i] << ' ';
    }
    os << '\n';
  }
  return os;
}
