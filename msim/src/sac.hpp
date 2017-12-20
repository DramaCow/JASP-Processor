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
      Line();
      Line(std::array<int,BLOCKSIZE> data, int baddr);
      Line(const Line &line);

      std::array<int,BLOCKSIZE> data;
      bool valid;
      int  baddr;
      int  tag;
      bool dirty;
      int  lru;
    };

  public:
    SAC(int n, int k);

    Line * access(int baddr);
    std::tuple<Line*,Line*> stash(int baddr, Line *line);

  private:
    std::vector<Line> lines;
    int numSets;
    int setSize;

    void adjustLRU(int idx, int l);
};

#endif
