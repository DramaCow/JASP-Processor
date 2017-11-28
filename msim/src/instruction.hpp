#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>
#include <iostream>

class Instruction
{
  public:
    Instruction();
    Instruction(std::string opcode);

    std::string opcode;
    std::vector<int> params;

    bool isArth(); // is logical/arithmetic instruction
    bool isBrch(); // is branch instruction
    bool isLdsr(); // is load/store instruction

    friend std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);
};

std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);

#endif
