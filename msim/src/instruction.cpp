#include "instruction.hpp"

Instruction::Instruction()
{
}

Instruction::Instruction(std::string opcode) : 
  opcode(opcode)
{
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
