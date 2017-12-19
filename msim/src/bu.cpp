#include "bu.hpp"

void BU::dispatch(BRS::Shelf shelf)
{
  this->shelf = shelf;

  if (this->shelf.opcode == "b")
  {
    this->taken = true;
    this->result = shelf.tgt;
    this->writeback = true;
  }
  else if (this->shelf.opcode == "beq")
  {
    this->taken = shelf.o1 == shelf.o2;
    this->result = this->taken ? shelf.tgt : shelf.npc;
    this->writeback = true;
  }
  else if (this->shelf.opcode == "bneq")
  {
    this->taken = shelf.o1 != shelf.o2;
    this->result = this->taken ? shelf.tgt : shelf.npc;
    this->writeback = true;
  }
  else if (this->shelf.opcode == "blt")
  {
    this->taken = shelf.o1 < shelf.o2;
    this->result = this->taken ? shelf.tgt : shelf.npc;
    this->writeback = true;
  }
  else if (this->shelf.opcode == "ble")
  {
    this->taken = shelf.o1 <= shelf.o2;
    this->result = this->taken ? shelf.tgt : shelf.npc;
    this->writeback = true;
  }
  else if (this->shelf.opcode == "bgt")
  {
    this->taken = shelf.o1 > shelf.o2;
    this->result = this->taken ? shelf.tgt : shelf.npc;
    this->writeback = true;
  }
  else if (this->shelf.opcode == "bge")
  {
    this->taken = shelf.o1 >= shelf.o2;
    this->result = this->taken ? shelf.tgt : shelf.npc;
    this->writeback = true;
  }
  // Unexpected OP
  else {
    this->writeback = false;
  }
}

void BU::reset()
{
  this->shelf.opcode = "nop";
  this->writeback = false;
}

std::ostream& operator<<(std::ostream& os, const BU& bu)
{
  if (bu.shelf.opcode == "nop")
  {
    os << "    --- nop ---\n";
    return os;
  }
  os << "    d" << bu.shelf.dest << " = (" << bu.shelf.o1 << ' ' << bu.shelf.opcode << ' ' << bu.shelf.o2 << ") ? " << bu.shelf.tgt << " : " << bu.shelf.npc << '\n';
  os << "    " << (bu.result != bu.shelf.pred ? "mispredicted X" : "correctly predicted O") << '\n';
  return os;
}
