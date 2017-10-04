#ifndef ALU_H
#define ALU_H

#include <cstdint>

#define ADD 0
#define SUB 1

class Alu
{
  public:
    Alu();
    void execute();

    uint32_t areg;
    uint32_t breg;
    uint32_t op;
    uint32_t res;
};

#endif
