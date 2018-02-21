#include "rs.hpp"
#include "instruction.hpp"
#include <array>
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

int RS::space() const
{
  //return this->shelves.size() >= NUM_RS_ENTRIES;
  return NUM_RS_ENTRIES - this->shelves.size();
}

void RS::issue(RS::Shelf shelf)
{
  if (this->shelves.size() >= NUM_RS_ENTRIES)
  {
    return;
  }
  this->shelves.push_back(shelf);
}

std::array<RS::Shelf,NUM_ALUS> RS::dispatch(RS &n_rs, std::array<bool,NUM_ALUS> port)
{
  std::array<RS::Shelf,NUM_ALUS> e;
  std::vector<RS::Shelf> shelves(this->shelves); // copy

  // only looks at the entries that exist this iteration AND next iteration
  // (i.e. for length of this iteration), else we may dispatch instructions
  // that were added just this iteration - which is impossible practically.
  for (std::size_t p = 0; p < NUM_ALUS; ++p)
  {
    if (port[p])
    { 
      for (std::size_t i = 0; i < shelves.size(); ++i)
      {
        if (shelves[i].v1 && shelves[i].v2)
        {
          e[p] = shelves[i];
          shelves.erase(std::begin(shelves) + i);
          n_rs.shelves.erase(std::begin(n_rs.shelves) + i);
          break;
        }
      }
    }
  }

  return e;
}

void RS::update(int dest, int result)
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

void RS::reset()
{
  this->shelves.clear();
}

RS& RS::operator=(const RS& rs)
{
  this->shelves = rs.shelves;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const RS& rs)
{
  os << "    space=" << rs.space() << '\n';
  os << "    OC    o1    o2    dest  \n";
  os << "    ------------------------\n";
  for (std::size_t i = 0; i < rs.shelves.size(); ++i)
  {
    os << "    ";
    os << SPACE(rs.shelves[i].opcode)
       << SPACE((rs.shelves[i].v1 ? std::string("") : std::string("d")) + std::to_string(rs.shelves[i].o1))
       << SPACE((rs.shelves[i].v2 ? std::string("") : std::string("d")) + std::to_string(rs.shelves[i].o2))
       << SPACE(std::string("d") + std::to_string(rs.shelves[i].dest))
       << '\n';
  }
  return os;
}
