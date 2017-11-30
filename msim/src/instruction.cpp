#include "instruction.hpp"

Instruction::Instruction()
{
}

Instruction::Instruction(std::string opcode) : 
  Instruction()
{
  this->opcode = opcode;
}

bool Instruction::isArth(std::string opcode)
{
  return opcode == "mov"  ||
         opcode == "movi" ||
         opcode == "add"  ||
         opcode == "addi" ||
         opcode == "sub"  ||
         opcode == "subi" ||
         opcode == "mul"  ||
         opcode == "muli" ||
         opcode == "xor";
}

bool Instruction::isBrch(std::string opcode)
{
  return opcode == "b"    ||
         opcode == "beq"  ||
         opcode == "bneq";
}

bool Instruction::isLdsr(std::string opcode)
{
  return false;
}

std::ostream& operator<<(std::ostream& os, const Instruction& Instruction)
{
  os << Instruction.opcode;
  for (const int &p : Instruction.params)
  {
    os << ' ' << p;
  }
  return os;
}
