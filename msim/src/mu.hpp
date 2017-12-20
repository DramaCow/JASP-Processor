#ifndef MU_H
#define MU_H

#include <string>
#include <vector>
#include <iostream>
#include "lsq.hpp"
#include "config.hpp"
#include "cache.hpp"
#include "sac.hpp"
#include "mem.hpp"

// Memory Unit
class MU
{
  public:
    MU(DCache &dcache, SAC &l1cache, SAC &l2cache, MEM &mem);

    bool isReady();

    void dispatch(LSQ::Shelf shelf);
    void execute(MU &n_mu);

    void reset();

    LSQ::Shelf shelf;
    int result;
    int duration = 0;
    bool writeback = false;

    MU& operator=(const MU& mu);

    friend std::ostream& operator<<(std::ostream& os, const MU& mu);

  private:
    DCache &dcache;
    SAC &l1cache;
    SAC &l2cache;
    MEM &mem;
    void l1_cachemiss();
    void l2_cachemiss();
    SAC::Line * getline(int baddr);
};
std::ostream& operator<<(std::ostream& os, const MU& mu);

#endif
