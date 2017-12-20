#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>
#include <vector>

#include "cache.hpp"
#include "hrt.hpp"
#include "pt.hpp"
#include "rat.hpp"
#include "rob.hpp"
#include "rrf.hpp"
#include "rs.hpp"
#include "brs.hpp"
#include "lsq.hpp"
#include "alu.hpp"
#include "bu.hpp"
#include "mu.hpp"
#include "sac.hpp"
#include "mem.hpp"

class Processor
{
  public:
    Processor(ICache &icache, SAC &l1cache, SAC &l2cache, MEM &mem);

    bool tick(Processor &n_cpu);

    ICache &icache; // Instruction cache
    SAC &l1cache;
    SAC &l2cache;
    MEM &mem;

    unsigned int pc = 0;

    //Instruction ibuf;
    std::array<std::tuple<int, Instruction>, FETCHRATE> ibuf;
    HRT hrt;
    PT pt;
    RAT rat;
    ROB rob;
    RRF rrf;
    RS  rs;
    BRS brs;
    LSQ lsq;
    std::array<ALU, NUM_ALUS> alu;
    BU  bu;
    MU  mu;

#ifdef EXE_TRACE
    std::vector<Instruction> exe;
#endif

    Processor& operator=(const Processor& cpu);

    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);

  private:
    // pipeline stage advancing functions
    void fetch(Processor &n_cpu);
    void decode(Processor &n_cpu);
    void execute(Processor &n_cpu);
    void writeback(Processor &n_cpu);
    bool commit(Processor &n_cpu);

    // helper functions
    std::tuple<int, bool> read(int r);
    int alloc(Processor &n_cpu, int pc, Instruction instruction, int reg);
    int alloc(Processor &n_cpu, int pc, Instruction instruction, int reg, bool pred, int pattern);
    bool isStalled();
    int space();
    void flush(int target);

    // statistics recorders
    int cycles;
    int instructions_executed;
    int branch_corpred;
    int branch_mispred;
    int instructions_fetched;
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
