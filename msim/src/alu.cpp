#include "alu.hpp"

// this is applyed to the next state
void ALU::dispatch(std::string opcode, int o1, int o2, int dest)
{
  this->opcode = opcode;
  this->o1 = o1;
  this->o2 = o2;
  this->dest = dest;

  if (this->opcode == "add" || this->opcode == "addi")
  {
    this->result = o1 + o2;
    this->duration = 0;
    this->writeback = true;
  }
  else if (this->opcode == "sub" || this->opcode == "subi")
  {
    this->result = o1 - o2;
    this->duration = 0;
    this->writeback = true;
  }
  else if (this->opcode == "xor")
  {
    this->result = o1 ^ o2;
    this->duration = 0;
    this->writeback = true;
  }
  else if (this->opcode == "mul" || this->opcode == "muli")
  {
    this->result = o1 * o2;
    this->duration = 2;
    this->writeback = true;
  }
  // Unexpected OP
  else {
    this->result = 0;
    this->duration = 0;
    this->writeback = false;
  }
}

// applied to the current state, updates the next state
void ALU::execute(ALU& n_alu)
{
  n_alu.duration = this->duration-1 > 0 ? this->duration-1 : 0;
}

void ALU::reset()
{
  this->opcode = "nop";
  this->writeback = false;
}

std::ostream& operator<<(std::ostream& os, const ALU& alu)
{
  if (alu.opcode == "nop")
  {
    os << "    --- nop ---\n";
    return os;
  }

  os << "    d" << alu.dest << " <-- " << alu.o1 << ' ' << alu.opcode << ' ' << alu.o2 << " (" << alu.duration << ")\n";
  if (alu.writeback && alu.duration == 0)
  {
    os << "    = " << alu.result << ' ' << "writeback" << '\n';
  }
  return os;
}
