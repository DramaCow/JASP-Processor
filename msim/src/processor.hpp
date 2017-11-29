#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "cache.hpp"
#include "bp.hpp"
#include "rat.hpp"
#include "rob.hpp"
#include "rrf.hpp"
#include "rs.hpp"
#include "alu.hpp"
#include "bu.hpp"

class Processor
{
  public:
    Processor(ICache &icache, DCache &dcache);

    void tick(Processor &n_cpu);

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
    ALU alu1;
    BU bu;

    Processor& operator=(const Processor& cpu);

    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);

  private:
    // pipeline stage advancing functions
    void fetch(Processor &n_cpu);
    void decode(Processor &n_cpu);
    void execute(Processor &n_cpu);
    void writeback(Processor &n_cpu);
    void commit(Processor &n_cpu);

    // helper functions
    std::tuple<int, bool> read(int r);
    int alloc(Processor &n_cpu, std::string opcode, int r, int val);
    bool isStalled();

    // statistics recorders
    int cycles;
    int instructions_executed;
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
