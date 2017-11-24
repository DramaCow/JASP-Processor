#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "cache.hpp"
#include "rat.hpp"
#include "rrf.hpp"
#include "rs.hpp"
#include "alu.hpp"
#include "bu.hpp"

class Processor
{
  public:
    Processor(ICache &icache, DCache &dcache);
    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);
    void tick(Processor &n_cpu);

    ICache &icache; // Instruction cache
    DCache &dcache; // data cache

    unsigned int pc = 0;

    Instruction ibuf;
    RAT rat;
    RRF rrf;
    RS rs;
    ALU alu1;
    //BU bu;

    bool isStalled();

    Processor& operator=(const Processor& cpu);

  private:
    // pipeline stage advancing functions
    void fetch(Processor &n_cpu);
    void decode(Processor &n_cpu);
    void dispatch(Processor &n_cpu);
    void execute(Processor &n_cpu);
    void writeback(Processor &n_cpu);
    void commit(Processor &n_cpu);

    // statistics recorders
    int cycles;
    int Instructions_executed;
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
