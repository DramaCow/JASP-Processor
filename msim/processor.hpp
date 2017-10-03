#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "memory.hpp"
#include "registers.hpp"

class Processor
{
  public:
    Processor(Memory &imem, Memory &dmem);

  private:
    Memory &imem; // instruction memory
    Memory &dmem; // data memory
    Registers regfile;
};

#endif
