#include "alu.hpp"

void Alu::execute(std::string opcode, int os1, int os2, int rd)
{
  this->opcode = opcode;
  this->os1 = os1;
  this->os2 = os2;
  this->rd = rd;

  if (opcode == "add" || opcode == "addi")
  {
    this->result = os1 + os2;
    this->duration = 0;
  }
  else if (opcode == "sub" || opcode == "subi")
  {
    this->result = os1 - os2;
    this->duration = 0;
  }
  else if (opcode == "xor")
  {
    this->result = os1 ^ os2;
    this->duration = 0;
  }
}
