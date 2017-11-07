#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "cache.hpp"
#include "registerfile.hpp"
#include "reservationstation.hpp"

class Processor
{
  public:
    Processor(ICache &icache, DCache &dcache);
    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);
    void tick(Processor &n_cpu);

    ICache &icache; // instruction cache
    DCache &dcache; // data cache

    unsigned int pc = 0;

    RegisterFile regfile;
    ReservationStation restat;

    bool isStalled();

    Processor& operator=(const Processor& cpu);

  private:
    struct Lat_f_d
    {
      Instruction instruction;
    };
    struct Lat_e_m
    {
      int result;
      int rd;
      bool we = false;
    };
    struct Lat_m_w
    {
      uint32_t data;
      uint32_t rd;
      bool we = false;
    }; 

  private:
    // between pipeline stage latches
    Lat_f_d lat_f_d;
    Lat_e_m lat_e_m;
    Lat_m_w lat_m_w;

    // pipeline stage advancing functions
    void fetch(Processor &n_cpu);
    void decode(Processor &n_cpu);
    void execute(Processor &n_cpu);
    void memaccess(Processor &n_cpu);
    void writeback(Processor &n_cpu);

    // statistics recorders
    int cycles;
    int instructions_executed;
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
