#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "memory.hpp"
#include "registerfile.hpp"
#include "alu.hpp"

class Processor
{
  public:
    Processor(Memory &imem, Memory &dmem);
    friend std::ostream& operator<<(std::ostream& os, const Processor& cpu);
    void tick();

  private:
    enum State { FETCH, DECODE, EXECUTE, WRITEBACK };
    State state;

    Memory &imem; // instruction memory
    Memory &dmem; // data memory
    
    RegisterFile regfile;
    uint32_t pc;
    uint32_t oreg;

    Alu alu;

    // pipeline functions
    void fetch();
    void decode();
    void execute();
    void writeback();
};

std::ostream& operator<<(std::ostream& os, const Processor& cpu);

#endif
