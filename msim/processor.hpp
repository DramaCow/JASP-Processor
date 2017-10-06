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
    struct Lat_if_id // 1
    {
      uint32_t npc; // next program counter (pc + 4)
      uint32_t op;  // operation and operand as 32bit word
    };
    struct Lat_id_exe // 2
    {
      uint32_t npc; // next program counter (propagated)
      uint32_t opcode;
      uint32_t a;   // value of register rs
      uint32_t b;   // value of register rt
      uint32_t imm; // immediate op value
      uint32_t rd;  // register id for writeback
    };
    struct Lat_exe_mem // 3
    {
      uint32_t npc;    // next program counter (propagated)
      uint32_t opcode; // (propagated)
      uint32_t t;      // result of execution
      uint32_t rd;     // register id for writback
    };
    struct Lat_mem_wb // 4
    {
      uint32_t t;   // result of execution (propagated)
      uint32_t mem; // data read from memory
      uint32_t rd;  // register id for writeback (propagated)
    }; 

  private:
    // pipeline state
    int state;

    enum OperationType { OP_ADD, OP_SUB, OP_XOR, OP_CMP, OP_GT, OP_LT, OP_EQ, OP_GE, OP_LE };

    // memory units (references)
    Memory &imem; // instruction memory
    Memory &dmem; // data memory

    RegisterFile regfile;
    uint32_t pc, npc;

    uint32_t oreg;       // fetch   --> decode    (operand register)
    uint32_t a_latch;    // decode  --> execute   (operand1)
    uint32_t b_latch;    // decode  --> execute   (operand2)
    OperationType otype; // decode  --> execute   (operation type)
    uint32_t waddr;      // decode  --> writeback (write address)
    bool we;             // decode  --> writeback (write enabled)
    bool abs_branch;     // decode  --> execute   (absolute branching - 0 = relative, 1 = abs)
    bool var_pc_inc;     // decode  --> execute   (variable PC increment - 0 = +4, 1 = +t_latch)
    uint32_t t_latch;    // execute --> writeback (result)
    uint32_t c_latch;    // execute --> execute-bypass (compare latch - | gt | lt | eq |)

    Address     address;
    Lat_if_id   lat_if_id;
    Lat_id_exe  lat_id_exe;
    Lat_exe_mem lat_exe_mem;
    Lat_mem_wb  lat_mem_wb;

    // pipeline stages
    void fetch();
    void decode();
    void execute();
    void writeback();

    // statistics recorders
    int cycles;
    int instructions_executed;

};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
