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
    SAC::Line * rL1(int baddr);
    SAC::Line * rL2(int baddr);
    SAC::Line * rMEM(int baddr);
    SAC::Line * wL1(int baddr);
    SAC::Line * wL2(int baddr);
    SAC::Line * wMEM(int baddr);
    SAC::Line * getline(int baddr);
};
std::ostream& operator<<(std::ostream& os, const MU& mu);

#endif
