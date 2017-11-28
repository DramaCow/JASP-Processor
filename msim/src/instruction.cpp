#include "instruction.hpp"

Instruction::Instruction()
{
}

Instruction::Instruction(std::string opcode) : 
  Instruction()
{
  this->opcode = opcode;
}

bool Instruction::isArth()
{
  return this->opcode == "add"  ||
         this->opcode == "addi" ||
         this->opcode == "sub"  ||
         this->opcode == "subi" ||
         this->opcode == "mul"  ||
         this->opcode == "muli" ||
         this->opcode == "xor";
}

bool Instruction::isBrch()
{
  return this->opcode == "b"    ||
         this->opcode == "beq"  ||
         this->opcode == "bneq";
}

bool Instruction::isLdsr()
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
