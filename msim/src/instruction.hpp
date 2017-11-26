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

    friend std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);
};

std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);

#endif
