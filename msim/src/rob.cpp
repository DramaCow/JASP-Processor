#include "rob.hpp"
#include <cstring>
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

int ROB::push(ROB &n_rob, std::string opcode, int r, int target)
{
  Entry::Type type = Entry::DN;
  if (Instruction::isArth(opcode) || opcode == "ld")
  {
    type = Entry::WB;
  }
  else if (Instruction::isBrch(opcode))
  {
    type = Entry::BR;
  }
  else if (opcode == "end")
  {
    type = Entry::END;
  }

  // alloc the rob entry
  n_rob.entries[this->head].type = type;
  n_rob.entries[this->head].reg = r;
  n_rob.entries[this->head].val = 0;
  n_rob.entries[this->head].target = target;
  n_rob.entries[this->head].done = opcode == "end";

  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int e = this->head + NUM_REGISTERS;

  // move the head (w/ wrap around)
  n_rob.head = (this->head + 1) % NUM_ROB_ENTRIES;

  return e;
}

std::vector<std::tuple<int,ROB::Entry>> ROB::pop(ROB &n_rob)
{
  std::vector<std::tuple<int,ROB::Entry>> commits;

  int c;
  for (c = 0; c < 4; ++c) // max commits per cycle is 4
  {
    int tail = (this->tail + c) % NUM_ROB_ENTRIES;
    if (!this->entries[tail].done || this->head == tail)
    {
      break;
    }
    commits.push_back(std::make_tuple(tail + NUM_REGISTERS, this->entries[tail]));
  }

  n_rob.tail = (this->tail + c) % NUM_ROB_ENTRIES;

  return commits;
}

std::tuple<int, bool> ROB::read(int addr)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  return std::make_tuple(this->entries[a].done ? this->entries[a].val : addr, this->entries[a].done);
}

void ROB::write(int addr, int val)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  this->entries[a].val = val;
  this->entries[a].done = true;
}

void ROB::reset()
{
  this->head = this->tail;
}

ROB& ROB::operator=(const ROB& rob)
{
  this->head = rob.head;
  this->tail = rob.tail;
  memcpy(this->entries, rob.entries, NUM_ROB_ENTRIES*sizeof(Entry));
  return *this;
}

std::ostream& operator<<(std::ostream& os, const ROB& rob)
{
  os << "    addr  type  reg   val   target\n";
  os << "    ------------------------------\n";
  for (int i = 0; i < NUM_ROB_ENTRIES; ++i)
  {
    os << "    ";
    os << SPACE(
      (rob.entries[i].done ? std::string("*") : std::string("")) + 
      std::string("d") + std::to_string(i+NUM_REGISTERS)
    );
    switch (rob.entries[i].type)
    {
      case ROB::Entry::DN: os << SPACE("DN"); break;
      case ROB::Entry::WB: os << SPACE("WB"); break;
      case ROB::Entry::BR: os << SPACE("BR"); break;
      case ROB::Entry::END: os << SPACE("END"); break;
      default: os << SPACE("??"); break;
    }

    if (rob.entries[i].reg >= 0)
    {
      os << SPACE(std::string("r") + std::to_string(rob.entries[i].reg));
    }
    else
    {
      os << SPACE("n/a");
    }

    os << SPACE(rob.entries[i].val);

    if (rob.entries[i].target >= 0)
    {
      os << SPACE(std::string("(") + std::to_string(rob.entries[i].target) + std::string(")"));
    }
    else 
    {
      os << SPACE("n/a");
    }

    if (rob.head == i)
    {
      os << " <-h-";
    }
    if (rob.tail == i)
    {
      os << " <-t-";
    }
    os << '\n';
  }
  return os;
}
