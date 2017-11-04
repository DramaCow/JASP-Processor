#include "instruction.hpp"

Instruction::Instruction() :
  params({0,0,0}),
  isConst({0,0,0})
{
}

Instruction::Instruction(std::string opcode) : 
  Instruction()
{
  this->opcode = opcode;
}

std::ostream& operator<<(std::ostream& os, const Instruction& instruction)
{
  os << instruction.opcode;
  for (const int &p : instruction.params)
  {
    os << ' ' << p;
  }
  return os;
}
