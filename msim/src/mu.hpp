#ifndef MU_H
#define MU_H

#include <string>
#include <vector>
#include <iostream>
#include "lsq.hpp"
#include "config.hpp"
#include "cache.hpp"

// Memory Unit
class MU
{
  public:
    MU(DCache &dcache);

    bool isReady();

    void dispatch(LSQ::Shelf shelf);
    void execute(MU &n_mu);

    MU& operator=(const MU& mu);

    friend std::ostream& operator<<(std::ostream& os, const MU& mu);

  private:
    LSQ::Shelf shelf;
    int result;
    int duration = 0;
    bool writeback = false;
    DCache &dcache;
};
std::ostream& operator<<(std::ostream& os, const MU& mu);

#endif
