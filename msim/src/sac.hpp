#ifndef SAC_H
#define SAC_H

#include "config.hpp"
#include <array>
#include <vector>
#include <tuple>

// Set-Associative Cache
class SAC
{
  public:
    struct Line
    {
      std::array<int,BLOCKSIZE> data;
      bool valid = false;
      int  tag   = 0;
      bool dirty = false;
    };

  public:
    SAC(int n, int k);

    std::tuple<int,bool> load(int addr);
    bool store(int addr, int val);

    std::array<int,BLOCKSIZE> getline(int a);
    void writeline(int a, Line line);

  private:
    std::vector<Line> lines;
    int setSize;
};

#endif
