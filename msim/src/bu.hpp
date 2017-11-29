#ifndef BU_H
#define BU_H

#include <string>
#include <iostream>

// Branching Unit
class BU
{
  public:
    void dispatch(std::string opcode, int target, int o1, int o2, bool pred, int dest);

    // input latch
    std::string opcode = "nop"; 
    int target = 0;
    int o1 = 0; 
    int o2 = 0; 
    bool pred = false;

    // output latch
    int dest = 0; 
    bool result = false;

    // control flags
    bool writeback = false;

    friend std::ostream& operator<<(std::ostream& os, const BU& bu);
};
std::ostream& operator<<(std::ostream& os, const BU& bu);

#endif
