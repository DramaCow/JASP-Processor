#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "memory.hpp"
#include "registers.hpp"

class Processor
{
  public:
    Processor(Memory &mem);

  private:
    Memory &mem;
    Registers regfile;
};

#endif
