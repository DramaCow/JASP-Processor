#ifndef ALU_H
#define ALU_H

#include <string>
#include <iostream>

// Arithmetic Logic Unit
class ALU
{
  public:
    void dispatch(std::string opcode, int os1, int os2, int rd);
    int execute(ALU& n_ALU);

    // input latch
    std::string opcode = "nop"; 
    int os1 = 0; 
    int os2 = 0; 
    int rd = 0;

    // ticks to complete Instruction execution
    int duration = 0; 

    // results latch
    int result = 0; 
    int dest = 0; 
    bool we = false; // writeback enabled

    friend std::ostream& operator<<(std::ostream& os, const ALU& ALU);
};
std::ostream& operator<<(std::ostream& os, const ALU& ALU);

#endif
