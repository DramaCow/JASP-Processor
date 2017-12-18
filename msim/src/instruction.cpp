#include "instruction.hpp"

Instruction::Instruction()
{
}

/*
Instruction::Instruction(std::string opcode) : 
  Instruction()
{
  this->opcode = opcode;
}
*/

int Instruction::getTakenBTA()
{
  int target = -1; // TODO: what happens if used incorrectly?

  if (this->opcode == "b")
  {
    target = this->params[0];
  }
  else if (this->opcode == "beq"  ||
           this->opcode == "bneq" ||
           this->opcode == "blt"  ||
           this->opcode == "ble"  ||
           this->opcode == "bgt"  ||
           this->opcode == "bge"     )
  {
    target = this->params[2];
  }

  return target;  
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
         opcode == "bneq" ||
         opcode == "blt"  ||
         opcode == "ble"  ||
         opcode == "bgt"  ||
         opcode == "ble";
}

bool Instruction::isLdsr(std::string opcode)
{
  return opcode == "lw" ||
         opcode == "sw";
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
