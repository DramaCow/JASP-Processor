#include "instruction.hpp"

Instruction::Instruction()
{
}

Instruction::Instruction(std::string opcode) : 
  Instruction()
{
  this->opcode = opcode;
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
