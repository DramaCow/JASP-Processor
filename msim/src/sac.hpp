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
      int lru = 0;
    };

  public:
    SAC(int n, int k);

    std::tuple<int,bool> load(int addr);
    bool store(int addr, int val);

    std::tuple<Line,bool> getline(int a);
    void writeline(int a, Line line);

  private:
    std::vector<Line> lines;
    int numSets;
    int setSize;

    void adjustLRU(int idx, int l);
};

#endif
