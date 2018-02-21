#ifndef MEM_H
#define MEM_H

#include "config.hpp"
#include <array>
#include <vector>
#include <iostream>

class MEM
{
  public:
    MEM(int *data, int size);

    std::array<int,BLOCKSIZE> getblock(int a);
    void writeblock(int a, std::array<int,BLOCKSIZE> block);

    friend std::ostream& operator<<(std::ostream& os, const MEM& mem);

  private:
    std::vector<std::array<int,BLOCKSIZE>> blocks;
};
std::ostream& operator<<(std::ostream& os, const MEM& mem);

#endif
