#include "brs.hpp"
#include "instruction.hpp"
#include <array>
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

int BRS::space() const
{
  return NUM_BRS_ENTRIES - this->shelves.size();
}

void BRS::issue(BRS::Shelf shelf)
{
  if (this->shelves.size() >= NUM_BRS_ENTRIES)
  {
    return;
  }
  this->shelves.push_back(shelf);
}

BRS::Shelf BRS::dispatch(BRS &n_brs, bool port)
{
  BRS::Shelf e;

  for (std::size_t i = 0; i < shelves.size(); ++i)
  {
    if (this->shelves[i].v1 && this->shelves[i].v2)
    {
      e = this->shelves[i];
      n_brs.shelves.erase(std::begin(n_brs.shelves) + i);
      break;
    }
  }

  return e;
}

void BRS::update(int dest, int result)
{
  for (std::size_t i = 0; i < this->shelves.size(); ++i)
  {
    if (!this->shelves[i].v1 && this->shelves[i].o1 == dest)
    {
      this->shelves[i].o1 = result;
      this->shelves[i].v1 = true;
    }

    if (!this->shelves[i].v2 && this->shelves[i].o2 == dest)
    {
      this->shelves[i].o2 = result;
      this->shelves[i].v2 = true;
    }
  }
}

void BRS::reset()
{
  this->shelves.clear();
}

BRS& BRS::operator=(const BRS& brs)
{
  this->shelves = brs.shelves;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const BRS& brs)
{
  os << "    space=" << brs.space() << '\n';
  os << "    OC    o1    o2    npc   tgt   dest  \n";
  os << "    ------------------------------------\n";
  for (std::size_t i = 0; i < brs.shelves.size(); ++i)
  {
    os << "    ";
    os << SPACE(brs.shelves[i].opcode)
       << SPACE((brs.shelves[i].v1 ? std::string("") : std::string("d")) + std::to_string(brs.shelves[i].o1))
       << SPACE((brs.shelves[i].v2 ? std::string("") : std::string("d")) + std::to_string(brs.shelves[i].o2))
       << SPACE(brs.shelves[i].npc)
       << SPACE(brs.shelves[i].tgt)
       << SPACE(std::string("d") + std::to_string(brs.shelves[i].dest))
       << '\n';
  }
  return os;
}
