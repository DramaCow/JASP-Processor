#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <array>

#include "cache.hpp"
#include "bp.hpp"
#include "rat.hpp"
#include "rob.hpp"
#include "rrf.hpp"
#include "rs.hpp"
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
    std::tuple<int, Instruction> ibuf;
    BP bp;
    RAT rat;
    ROB rob;
    RRF rrf;
    RS rs;
    LSQ lsq;
    //std::array<ALU, NUM_ALUS> alu;
    ALU alu;
    BU bu;
    MU mu;

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
    int alloc(Processor &n_cpu, std::string opcode, int r, int target);
    bool isStalled();
    void flush(int target);

    // statistics recorders
    int cycles;
    int instructions_executed;
    int branch_corpred;
    int branch_mispred;
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
