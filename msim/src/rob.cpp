#include "rob.hpp"
#include <cstring>

std::tuple<int, bool> ROB::read(int addr)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  return std::make_tuple(this->done[a] ? this->val[a] : addr, this->done[a]);
}

int ROB::push(ROB &n_rob, int r)
{
  // alloc the rob entry
  n_rob.reg[this->head] = r;
  n_rob.val[this->head] = 0;
  n_rob.done[this->head] = false;

  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int e = this->head + NUM_REGISTERS;

  // move the head (w/ wrap around)
  n_rob.head = (this->head + 1) % NUM_ROB_ENTRIES;

  return e;
}

void ROB::write(int addr, int val)
{
  // NOTE: rob addresses are offset by NUM_REGISTERS in order
  //       to differentiate them fron architectural addresses
  int a = addr - NUM_REGISTERS;
  this->val[a] = val;
  this->done[a] = true;
}

std::vector<std::tuple<int,int,int>> ROB::pop(ROB &n_rob)
{
  std::vector<std::tuple<int,int,int>> commits;

  int c;
  for (c = 0; c < 4; ++c) // max commits per cycle is 4
  {
    int tail = (this->tail + c) % NUM_REGISTERS;
    if (!this->done[tail] || this->head == tail)
    {
      break;
    }
    commits.push_back(std::make_tuple(this->reg[tail], this->val[tail], tail));
  }

  n_rob.tail = (this->tail + c) % NUM_REGISTERS;

  return commits;
}

ROB& ROB::operator=(const ROB& rob)
{
  this->head = rob.head;
  this->tail = rob.tail;
  memcpy(this->reg, rob.reg, NUM_ROB_ENTRIES*sizeof(int));
  memcpy(this->val, rob.val, NUM_ROB_ENTRIES*sizeof(int));
  memcpy(this->done, rob.done, NUM_ROB_ENTRIES*sizeof(bool));
  return *this;
}

std::ostream& operator<<(std::ostream& os, const ROB& rob)
{
  for (int i = 0; i < NUM_ROB_ENTRIES; ++i)
  {
    os << "    " << i+NUM_REGISTERS << ": " << rob.reg[i] << ", " << rob.val[i] << ", " << rob.done[i];
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
