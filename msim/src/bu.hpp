#ifndef BU_H
#define BU_H

#include <string>
#include <iostream>

// Branching Unit
class BU
{
  public:
    void dispatch(std::string opcode, int o1, int o2, int target);
    int execute(BU &n_BU);

    friend std::ostream& operator<<(std::ostream& os, const BU& BU);

  private:
    // input latch variables
    std::string opcode = "nop";
    int o1 = 0;
    int o2 = 0;
    int target = 0;

    // meta-control variables
    int duration = 0;

    // output latch variables
    int addr = 0; // target address
    bool cond = false; // branch condition (pc = addr if cond else pc + 1)
};
std::ostream& operator<<(std::ostream& os, const BU& BU);

#endif
