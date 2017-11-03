#include "instruction.hpp"

Instruction::Instruction()
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
