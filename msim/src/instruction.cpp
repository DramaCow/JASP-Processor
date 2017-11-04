#include "instruction.hpp"

Instruction::Instruction() :
  params({0,0,0}),

  isReg({0,0,0}),
  printOrder({0,1,2})
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
  for (int i = 0; i < instruction.num_params; ++i)
  {
    os << ' ';
    if (instruction.isReg[instruction.printOrder[i]])
    {
      os << 'r';
    }
    os << instruction.params[instruction.printOrder[i]];
  }
  return os;
}
