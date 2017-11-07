#ifndef ALU_H
#define ALU_H

#include <string>

class Alu
{
  public:
    void execute(std::string opcode, int os1, int os2, int rd);

  private:
    std::string opcode; int os1; int os2;
    int duration; // t-1 to complete instruction execution
    int result; int rd; // results latch
};

#endif
