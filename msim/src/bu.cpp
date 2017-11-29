#include "bu.hpp"

void BU::dispatch(std::string opcode, bool pred, int o1, int o2, int dest)
{
  this->opcode = opcode;
  this->pred = pred;
  this->o1 = o1;
  this->o2 = o2;
  this->dest = dest;

  if (this->opcode == "b")
  {
    this->result = this->pred != true;
    this->writeback = true;
  }
  if (this->opcode == "beq")
  {
    this->result = this->pred != (this->o1 == this->o2);
    this->writeback = true;
  }
  if (this->opcode == "bneq")
  {
    this->result = this->pred != (this->o1 != this->o2);
    this->writeback = true;
  }
  // Unexpected OP
  else {
    this->result = false;
    this->writeback = false;
  }
}

std::ostream& operator<<(std::ostream& os, const BU& bu)
{
  if (bu.opcode == "nop")
  {
    os << "    --- nop ---\n";
    return os;
  }
  os << "    " << bu.opcode << ' ' << bu.pred << ' ' << bu.o1 << ' ' << bu.o2 << '\n';
  os << "    " << "= " << bu.result << '\n';
  return os;
}
