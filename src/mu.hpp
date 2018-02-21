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
    MU(SAC &l1cache, SAC &l2cache, MEM &mem);

    bool isReady();

    void dispatch(LSQ::Shelf shelf);
    void execute(MU &n_mu);

    void complete_writethrough();
    void reset();

    LSQ::Shelf shelf;
    int result;
    int duration = 0;
    bool writeback = false;

    int l1_accesses = 0;
    int l2_accesses = 0;
    int mem_accesses = 0;

    MU& operator=(const MU& mu);

    friend std::ostream& operator<<(std::ostream& os, const MU& mu);

  private:
    SAC &l1cache;
    SAC &l2cache;
    MEM &mem;

    int  load(int addr);
    void store(int addr, int val);

#if CACHELEVELS != 1
    SAC::Line * rL1(int baddr);
    SAC::Line * rL2(int baddr);
    SAC::Line * rMEM(int baddr);
    SAC::Line * wL1(SAC::Line *line);
    SAC::Line * wL2(SAC::Line *line);
    void bL2(SAC::Line *line);
    void bMEM(SAC::Line *line);
#else
    SAC::Line * rL1(int baddr);
    SAC::Line * rMEM(int baddr);
    SAC::Line * wL1(SAC::Line *line);
    void bMEM(SAC::Line *line);
#endif
};
std::ostream& operator<<(std::ostream& os, const MU& mu);

#endif
