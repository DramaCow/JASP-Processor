#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "cache.hpp"
#include "registerfile.hpp"

class Processor
{
  public:
    Processor(ICache &icache, DCache &dcache);
    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);
    void tick(Processor &n_cpu);

    // general purpose registers interface
    RegisterFile regfile;

  private:
    struct Address
    {
      uint32_t pc = 0; // program counter
    };
    struct Lat_f_d
    {
      uint32_t npc = 16; // next program counter (pc + 4)
      uint32_t oreg = 0; // operation and operand as 32bit word
    };
    struct Lat_d_e
    {
      uint32_t npc = 12;   // next program counter (propagated)
      uint32_t opcode = 0;
      uint32_t a = 0;      // value of register rs
      uint32_t b = 0;      // value of register rt
      uint32_t imm = 0;    // immediate op value
      uint32_t rdest = 0;  // register id for writeback
    };
    struct Lat_e_m
    {
      uint32_t npc = 8;    // next program counter (propagated)
      uint32_t opcode = 0; // (propagated)
      uint32_t cmp = 0;    // comparison flag (occurs automatically)
      uint32_t t = 0;      // result of execution
      uint32_t b = 0;      // second register value read (see diagram)
      uint32_t rdest = 0;  // register id for writback
    };
    struct Lat_m_w
    {
      uint32_t npc = 4;    // next program counter (propagated)
      uint32_t opcode = 0; // (propagated)
      uint32_t data = 0;   // result of execution (propagated)
      uint32_t rdest = 0;  // register id for writeback (propagated)
    }; 

  private:
    // cache units (references)
    ICache &icache; // instruction cache
    DCache &dcache; // data cache

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
