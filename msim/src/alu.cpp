#include "alu.hpp"

// this is applyed to the next state
void ALU::dispatch(std::string opcode, int os1, int os2, int rd)
{
  this->duration = 0;

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

// applied to the current state, updates the next state
int ALU::execute(ALU& n_ALU)
{
  n_ALU.we = false;

  if (this->duration == 0)
  {
    return 0;
  }

  // about to finish
  if (n_ALU.duration <= 1)
  {
    if (this->opcode == "add" || this->opcode == "addi")
    {
      n_ALU.result = this->os1 + this->os2;
    }
    else if (this->opcode == "sub" || this->opcode == "subi")
    {
      n_ALU.result = this->os1 - this->os2;
    }
    else if (this->opcode == "xor")
    {
      n_ALU.result = this->os1 ^ this->os2;
    }
    else if (this->opcode == "mul" || this->opcode == "muli")
    {
      n_ALU.result = this->os1 * this->os2;
    }
    n_ALU.dest = this->rd;
    n_ALU.we = true;
    return 1;
  }
  
  n_ALU.duration = this->duration - 1;
  return 0;
}

std::ostream& operator<<(std::ostream& os, const ALU& ALU)
{
  os << "    IN: " << ALU.opcode << ' ' << ALU.os1 << ' ' << ALU.os2 << ' ' << ALU.rd << '\n';
  os << "    duration = " << ALU.duration << '\n';
  os << "    OUT: " << ALU.result << ' ' << ALU.dest << '\n';
  os << "    we = " << ALU.we << '\n';
  return os;
}
