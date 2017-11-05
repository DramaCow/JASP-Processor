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
    friend std::ostream& operator<<(std::ostream& os, const Instruction& instruction);

    std::string opcode;
    std::vector<int> params;
};

std::ostream& operator<<(std::ostream& os, const Instruction& instruction);

#endif
