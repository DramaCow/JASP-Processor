#ifndef ALU_H
#define ALU_H

#include <string>
#include <iostream>

class Alu
{
  public:
    void dispatch(std::string opcode, int os1, int os2, int rd);
    int execute(Alu& n_alu);
    friend std::ostream& operator<<(std::ostream& os, const Alu& alu);

    // input latch
    std::string opcode = "nop"; 
    int os1 = 0; 
    int os2 = 0; 
    int rd = 0;

    // ticks to complete instruction execution
    int duration = 0; 

    // results latch
    int result = 0; 
    int dest = 0; 
    bool we = false; // writeback enabled
};
std::ostream& operator<<(std::ostream& os, const Alu& alu);

#endif
