#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "memory.hpp"
#include "registerfile.hpp"

class Processor
{
  public:
    Processor(Memory &imem, Memory &dmem);
    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);
    void tick();

  private:
    // between pipeline state latches
    struct Address // 4
    {
      uint32_t pc = 0; // program counter
    };
    struct Lat_f_d // 1
    {
      uint32_t npc;  // next program counter (pc + 4)
      uint32_t oreg; // operation and operand as 32bit word
    };
    struct Lat_d_e // 2
    {
      uint32_t npc;   // next program counter (propagated)
      uint32_t opcode;
      uint32_t a;     // value of register rs
      uint32_t b;     // value of register rt
      uint32_t imm;   // immediate op value
      uint32_t rdest; // register id for writeback
    };
    struct Lat_e_m // 3
    {
      uint32_t npc;    // next program counter (propagated)
      uint32_t opcode; // (propagated)
      uint32_t t;      // result of execution
      uint32_t cmp;    // comparison flag (occurs automatically)
      uint32_t rdest;  // register id for writback
    };
    struct Lat_m_w // 4
    {
      uint32_t t;     // result of execution (propagated)
      uint32_t mem;   // data read from memory
      uint32_t rdest; // register id for writeback (propagated)
    }; 

  private:
    // pipeline state
    int state;

    // memory units (references)
    Memory &imem; // instruction memory
    Memory &dmem; // data memory

    RegisterFile regfile;

    Address address;
    Lat_f_d lat_f_d;
    Lat_d_e lat_d_e;
    Lat_e_m lat_e_m;
    Lat_m_w lat_m_w;

    // pipeline stages
    void fetch();
    void decode();
    void execute();
    void memaccess();
    void writeback();

    // statistics recorders
    int cycles;
    int instructions_executed;

};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
