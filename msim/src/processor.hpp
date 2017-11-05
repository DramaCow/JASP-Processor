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

    RegisterFile regfile;
    ReservationStation restat;

    Processor& operator=(const Processor& cpu);

  private:
    struct Address
    {
      unsigned int pc = 0;
    };
    struct Lat_f_d
    {
      unsigned int pc = 4;
      Instruction instruction;
    };
    struct Lat_d_e
    {
      unsigned int pc = 3;
      bool we = false;
    };
    struct Lat_e_m
    {
      unsigned int pc = 2;
      int result;
      int rd;
      bool we = false;
    };
    struct Lat_m_w
    {
      unsigned int pc = 1;
      uint32_t data;
      uint32_t rd;
      bool we = false;
    }; 

  private:
    // between pipeline stage latches
    Address address;
    Lat_f_d lat_f_d;
    Lat_d_e lat_d_e;
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
