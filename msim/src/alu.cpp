#include "alu.hpp"

void Alu::dispatch(std::string opcode, int os1, int os2, int rd)
{
  this->opcode = opcode;
  this->os1 = os1;
  this->os2 = os2;
  this->rd = rd;

  if (opcode == "add" || opcode == "addi" || opcode == "sub" || opcode == "subi" || opcode == "xor")
  {
    this->duration = 1;
  }
  else if (opcode == "mul" || opcode == "muli")
  {
    this->duration = 3;
  }
}

void Alu::execute()
{
  if (this->duration == 0)
  {
    return;
  }

  duration--;

  if (this->duration == 0)
  {
    if (opcode == "add" || opcode == "addi")
    {
      this->result = os1 + os2;
    }
    else if (opcode == "sub" || opcode == "subi")
    {
      this->result = os1 - os2;
    }
    else if (opcode == "xor")
    {
      this->result = os1 ^ os2;
    }
    else if (opcode == "mul" || opcode == "muli")
    {
      this->result = os1 * os2;
    }
    this->we = true;

    // not necessary, but useful for debugging
    this->opcode = "nop";
  }
}
