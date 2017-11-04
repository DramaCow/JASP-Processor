#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <array>
#include <iostream>

class Instruction
{
  public:
    Instruction();
    Instruction(std::string opcode);
    friend std::ostream& operator<<(std::ostream& os, const Instruction& instruction);

    std::string opcode;
    std::array<int,3> params;

    int num_params;
    std::array<bool,3> isReg;
    std::array<int,3> printOrder;
};

std::ostream& operator<<(std::ostream& os, const Instruction& instruction);

#endif
