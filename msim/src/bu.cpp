#include "bu.hpp"

void BU::dispatch(std::string opcode, int os1, int os2, int target)
{
  this->opcode = opcode;
  this->os1 = os1;
  this->os2 = os2;
  this->target = target;

  this->duration = 0;

  if (opcode == "b" || opcode == "beq" || opcode == "bneq")
  {
    this->duration = 1;
  }
}

int BU::execute(BU &n_BU)
{
  n_BU.cond = false;

  if (this->duration == 0)
  {
    return 0;
  }

  // about to finish
  if (n_BU.duration <= 1)
  {
    if (this->opcode == "b")
    {
      n_BU.cond = true;
    }
    else if (this->opcode == "beq")
    {
      n_BU.cond = this->os1 == this->os2;
    }
    else if (this->opcode == "bneq")
    {
      n_BU.cond = this->os1 != this->os2;
    }
    n_BU.addr = this->target;
    return 1;
  }
  
  n_BU.duration = this->duration - 1;
  return 0;
}

std::ostream& operator<<(std::ostream& os, const BU& BU)
{
  os << "    IN: " << BU.opcode << ' ' << BU.os1 << ' ' << BU.os2 << ' ' << BU.target;
  os << "    duration = " << BU.duration << '\n';
  os << "    OUT: " << BU.cond << ' ' << BU.addr << '\n';
  return os;
}
