#ifndef SAC_H
#define SAC_H

#include "config.hpp"
#include <array>
#include <vector>
#include <tuple>
#include <iostream>

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

    std::vector<Line> lines; // TODO: ideally not public (necessary for complete_writethrough)

    friend std::ostream& operator<<(std::ostream& os, const SAC& sac);

  private:
    int numSets;
    int setSize;

    void adjustLRU(int idx, int l);
};
std::ostream& operator<<(std::ostream& os, const SAC& sac);

#endif
