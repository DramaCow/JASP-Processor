#ifndef ALU_H
#define ALU_H

#include <cstdint>

#define OP_ADD 0
#define OP_SUB 1
#define OP_XOR 2

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
