#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "memory.h"
#include "registers.h"

class Processor
{
  public:
    Processor(Memory &mem);

  private:
    Memory &mem;
    Registers regfile;
};

#endif
