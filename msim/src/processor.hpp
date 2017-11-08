#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "cache.hpp"
#include "registerfile.hpp"
#include "reservationstation.hpp"
#include "alu.hpp"

class Processor
{
  public:
    Processor(ICache &icache, DCache &dcache);
    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);
    void tick(Processor &n_cpu);

    ICache &icache; // instruction cache
    DCache &dcache; // data cache

    unsigned int pc = 0;

    Instruction instbuf;
    RegisterFile regfile;
    ReservationStation restat;
    Alu alu1;

    bool isStalled();

    Processor& operator=(const Processor& cpu);

  private:
    // pipeline stage advancing functions
    void fetch(Processor &n_cpu);
    void decode(Processor &n_cpu);
    void dispatch(Processor &n_cpu);
    void execute(Processor &n_cpu);
    void writeback(Processor &n_cpu);

    // statistics recorders
    int cycles;
    int instructions_executed;
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
