#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>
#include <vector>

#include "cache.hpp"
#include "bp.hpp"
#include "bht.hpp"
#include "rat.hpp"
#include "rob.hpp"
#include "rrf.hpp"
#include "rs.hpp"
#include "brs.hpp"
#include "lsq.hpp"
#include "alu.hpp"
#include "bu.hpp"
#include "mu.hpp"

class Processor
{
  public:
    Processor(ICache &icache, DCache &dcache);

    bool tick(Processor &n_cpu);

    ICache &icache; // Instruction cache
    DCache &dcache; // data cache

    unsigned int pc = 0;

    //Instruction ibuf;
    std::array<std::tuple<int, Instruction>, FETCHRATE> ibuf;
    BP bp;
    BHT bht;
    RAT rat;
    ROB rob;
    RRF rrf;
    RS rs;
    BRS brs;
    LSQ lsq;
    std::array<ALU, NUM_ALUS> alu;
    //ALU alu;
    BU bu;
    MU mu;

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
    int alloc(Processor &n_cpu, int pc, Instruction instruction, int r, int target);
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
