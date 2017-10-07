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
      uint32_t npc = 0;  // next program counter (pc + 4)
      uint32_t oreg = 0; // operation and operand as 32bit word
    };
    struct Lat_d_e // 2
    {
      uint32_t npc = 0;    // next program counter (propagated)
      uint32_t opcode = 0;
      uint32_t a = 0;      // value of register rs
      uint32_t b = 0;      // value of register rt
      uint32_t imm = 0;    // immediate op value
      uint32_t rdest = 0;  // register id for writeback
    };
    struct Lat_e_m // 3
    {
      uint32_t npc = 0;    // next program counter (propagated)
      uint32_t opcode = 0; // (propagated)
      uint32_t t = 0;      // result of execution
      uint32_t cmp = 0;    // comparison flag (occurs automatically)
      uint32_t rdest = 0;  // register id for writback
    };
    struct Lat_m_w // 4
    {
      uint32_t opcode = 0; // (propagated)
      uint32_t data = 0;   // result of execution (propagated)
      uint32_t rdest = 0;  // register id for writeback (propagated)
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
