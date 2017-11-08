#ifndef ALU_H
#define ALU_H

#include <string>

class Alu
{
  public:
    void dispatch(std::string opcode, int os1, int os2, int rd);
    void execute();

  private:
    std::string opcode; int os1; int os2; // input latch
    int duration; // ticks to complete instruction execution
    int result; int rd; // results latch
    bool we = false; // writeback enabled
};

#endif
