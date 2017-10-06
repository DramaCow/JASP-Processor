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
    // pipeline state
    int state;

    enum OperationType { OP_ADD, OP_SUB, OP_XOR };

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
    uint32_t t_latch;    // execute --> writeback (result)

    // pipeline stages
    void fetch();
    void decode();
    void execute();
    void writeback();

    // statistics recorders
    int cycles;
    int instructions_executed;

  private:
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
