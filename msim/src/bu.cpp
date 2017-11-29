#include "bu.hpp"

void BU::dispatch(std::string opcode, int target, int o1, int o2, bool pred, int dest)
{
  this->opcode = opcode;
  this->target = target;
  this->o1 = o1;
  this->o2 = o2;
  this->pred = pred;
  this->dest = dest;

  if (this->opcode == "b")
  {
    this->result = pred == true;
    this->writeback = true;
  }
  if (this->opcode == "beq")
  {
    this->result = pred == (this->o1 == this->o2);
    this->writeback = true;
  }
  if (this->opcode == "bneq")
  {
    this->result = pred == (this->o1 != this->o2);
    this->writeback = true;
  }
  // Unexpected OP
  else {
    this->result = 0;
    this->writeback = false;
  }
}

std::ostream& operator<<(std::ostream& os, const BU& bu)
{
  os << "    " << bu.opcode << ' ' << bu.target << ' ' << bu.o1 << ' ' << bu.o2 << ' ' << bu.pred;
  return os;
}
