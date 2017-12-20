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

    MU& operator=(const MU& mu);

    friend std::ostream& operator<<(std::ostream& os, const MU& mu);

  private:
    SAC &l1cache;
    SAC &l2cache;
    MEM &mem;

    int  load(int addr);
    void store(int addr, int val);

    // writeback statemachine functions
    SAC::Line * rL1(int baddr);
    SAC::Line * rL2(int baddr);
    SAC::Line * rMEM(int baddr);
    SAC::Line * wL1(SAC::Line *line);
    SAC::Line * wL2(SAC::Line *line);
    void bL2(SAC::Line *line);
    void bMEM(SAC::Line *line);
};
std::ostream& operator<<(std::ostream& os, const MU& mu);

#endif
