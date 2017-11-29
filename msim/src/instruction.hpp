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

    static bool isArth(std::string opcode); // is logical/arithmetic instruction
    static bool isBrch(std::string opcode); // is branch instruction
    static bool isLdsr(std::string opcode); // is load/store instruction

    friend std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);
};

std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);

#endif
