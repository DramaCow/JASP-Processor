#include "alu.hpp"

// this is applyed to the next state
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
  else
  {
    this->duration = 0;
  }
}

// applied to the current state, updates the next state
int Alu::execute(Alu& n_alu)
{
  n_alu.we = false;

  if (this->duration == 0)
  {
    return 0;
  }

  // about to finish
  if (n_alu.duration <= 1)
  {
    if (this->opcode == "add" || this->opcode == "addi")
    {
      n_alu.result = this->os1 + this->os2;
    }
    else if (this->opcode == "sub" || this->opcode == "subi")
    {
      n_alu.result = this->os1 - this->os2;
    }
    else if (this->opcode == "xor")
    {
      n_alu.result = this->os1 ^ this->os2;
    }
    else if (this->opcode == "mul" || this->opcode == "muli")
    {
      n_alu.result = this->os1 * this->os2;
    }
    n_alu.dest = this->rd;
    n_alu.we = true;
    return 1;
  }
  
  n_alu.duration = this->duration - 1;
  return 0;
}

std::ostream& operator<<(std::ostream& os, const Alu& alu)
{
  os << "    IN: " << alu.opcode << ' ' << alu.os1 << ' ' << alu.os2 << ' ' << alu.rd << '\n';
  os << "    duration = " << alu.duration << '\n';
  os << "    OUT: " << alu.result << ' ' << alu.dest << '\n';
  os << "    we = " << alu.we << '\n';
  return os;
}
