#ifndef ALU_H
#define ALU_H

#include <string>
#include <iostream>

// Arithmetic Logic Unit
class ALU
{
  public:
    void dispatch(std::string opcode, int o1, int o2, int dest);
    void execute(ALU& n_alu);

    void reset();

    // input latch
    std::string opcode = "nop"; 
    int o1 = 0; 
    int o2 = 0; 

    // output latch
    int dest = 0; 
    int result = 0; 

    // ticks to complete instruction execution
    int duration = 0; 

    // control flags
    bool writeback = false;

    friend std::ostream& operator<<(std::ostream& os, const ALU& ALU);
};
std::ostream& operator<<(std::ostream& os, const ALU& ALU);

#endif
