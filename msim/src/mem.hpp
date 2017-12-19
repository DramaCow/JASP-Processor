#ifndef MEM_H
#define MEM_H

#include "config.hpp"
#include <array>

class MEM
{
  public:
    MEM();
    MEM(int *data);

    std::array<int,BLOCKSIZE> getblock(int a);
    void writeblock(int a, std::array<int,BLOCKSIZE> block);

  private:
    std::array<std::array<int,BLOCKSIZE>,MEM_NUM_BLOCKS> blocks;
};

#endif
