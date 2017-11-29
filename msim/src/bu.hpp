#ifndef BU_H
#define BU_H

#include <string>
#include <iostream>

// Branching Unit
class BU
{
  public:
    void dispatch(std::string opcode, bool pred, int o1, int o2, int dest);

    // input latch
    std::string opcode = "nop"; 
    bool pred = false;
    int o1 = 0; 
    int o2 = 0; 

    // output latch
    int dest = 0; 
    bool result = false;

    // control flags
    bool writeback = false;

    friend std::ostream& operator<<(std::ostream& os, const BU& bu);
};
std::ostream& operator<<(std::ostream& os, const BU& bu);

#endif
