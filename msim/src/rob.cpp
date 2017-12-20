#include "rob.hpp"
#include <cstring>
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

int ROB::space() const
{
  return NUM_ROB_ENTRIES - this->size - 1;
}

int ROB::push(ROB &n_rob, int pc, Instruction instruction, int reg, bool pred, int pattern)
{
  std::string opcode = instruction.opcode;

  Entry::Type type = Entry::DN;
  if (Instruction::isArth(opcode) || opcode == "lw")
  {
    type = Entry::WB;
  }
  else if (Instruction::isBrch(opcode))
  {
    type = Entry::BR;
  }
  else if (opcode == "sw")
  {
    type = Entry::SR;
  }
  else if (opcode == "end")
  {
    type = Entry::END;
  }

  // alloc the rob entry
  n_rob.entries[this->head].type = type;
  n_rob.entries[this->head].pc = pc;
  n_rob.entries[this->head].reg = reg;
  n_rob.entries[this->head].val = 0;
  n_rob.entries[this->head].pred = pred;
  n_rob.entries[this->head].pattern = pattern;
  n_rob.entries[this->head].done = opcode == "end";
  n_rob.entries[this->head].instruction = instruction;
  
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int e = this->head + NUM_REGISTERS;

  // move the head (w/ wrap around)
  n_rob.head = (this->head + 1) % NUM_ROB_ENTRIES;

  n_rob.size = this->size + 1;

  return e;
}

std::vector<std::tuple<int,ROB::Entry>> ROB::pop(ROB &n_rob, LSQ &n_lsq)
{
  std::vector<std::tuple<int,ROB::Entry>> commits;

  int c;
  for (c = 0; c < RETIRERATE; ++c)
  {
    int tail = (this->tail + c) % NUM_ROB_ENTRIES;
    if (!this->entries[tail].done || this->head == tail)
    {
      break;
    }
    commits.push_back(std::make_tuple(tail + NUM_REGISTERS, this->entries[tail]));
    n_rob.size--;
  }

  // TODO: mom's spaghetti
  int tail = (this->tail + c) % NUM_ROB_ENTRIES;
  if (this->head != tail && this->entries[tail].type == ROB::Entry::SR)
  {
    n_lsq.mark(tail + NUM_REGISTERS);
  }

  n_rob.tail = tail;

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

void ROB::write(int addr, int val, bool t)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  this->entries[a].val = val;
  this->entries[a].taken = t;
  this->entries[a].done = true;
}

void ROB::reset()
{
  this->head = this->tail;
  this->size = 0;
}

void ROB::set_spec(int seq, bool spec)
{
  int a = seq - NUM_REGISTERS;
  this->entries[a].spec = spec;
}

int ROB::get_tail()
{
  return tail;
}

ROB& ROB::operator=(const ROB& rob)
{
  this->head = rob.head;
  this->tail = rob.tail;
  this->size = rob.size;
  this->entries = rob.entries;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const ROB& rob)
{
  os << "    space=" << rob.space() << '\n';
  os << "    addr  type  pc    reg   val   p/t   patt  spec  \n";
  os << "    ------------------------------------------------\n";
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
      case ROB::Entry::SR: os << SPACE("SR"); break;
      case ROB::Entry::END: os << SPACE("END"); break;
      default: os << SPACE("??"); break;
    }

    if (rob.entries[i].pc >= 0)
    {
      os << SPACE(rob.entries[i].pc);
    }
    else
    {
      os << SPACE("--");
    }

    if (rob.entries[i].reg >= 0)
    {
      os << SPACE(std::string("r") + std::to_string(rob.entries[i].reg));
    }
    else
    {
      os << SPACE("--");
    }

    os << SPACE(rob.entries[i].val);

    if (rob.entries[i].type == ROB::Entry::BR)
    {
      os << SPACE(std::to_string(rob.entries[i].pred) + std::string(" ") + (rob.entries[i].done ? std::to_string(rob.entries[i].taken) : std::string("-")));
    }
    else
    {
      os << SPACE("- -");
    }

    if (rob.entries[i].type == ROB::Entry::BR)
    {
      os << SPACE(rob.entries[i].pattern);
    }
    else
    {
      os << SPACE("--");
    }

    if (rob.entries[i].spec)
    {
      os << SPACE("yes");
    }
    else
    {
      os << SPACE("no");
    }

    if (rob.head == i)
    {
      os << " <-h-(issue)";
    }
    if (rob.tail == i)
    {
      os << " <-t-(retire)";
    }
    os << '\n';
  }
  return os;
}
