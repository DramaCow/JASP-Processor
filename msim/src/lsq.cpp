#include "lsq.hpp"
#include <iomanip>
#include <algorithm>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

void LSQ::issue(Shelf shelf, int tail)
{
  std::size_t i;
  for (i = 0; i < this->shelves.size(); ++i)
  {
    int s1 = shelf.seq-NUM_REGISTERS + (shelf.seq-NUM_REGISTERS < tail ? NUM_ROB_ENTRIES : 0);
    int s2 = this->shelves[i].seq-NUM_REGISTERS + (this->shelves[i].seq-NUM_REGISTERS < tail ? NUM_ROB_ENTRIES : 0);

    if (s1 < s2)
    {
      std::cout << s1 << ' ' << s2 << '\n';
      std::cout << shelf.seq << ' ' << this->shelves[i].seq << '\n';
      break;
    }
  }
  this->shelves.insert(std::begin(this->shelves) + i, shelf);
  this->isNew.insert(std::begin(this->isNew) +i, true);
}

LSQ::Shelf LSQ::dispatch(LSQ &n_lsq, bool port)
{
  Shelf e;

  if (port)
  {
    //for (std::size_t i = 0; i < n_lsq.shelves.size(); ++i)
    for (int i = 0; i < std::min(1, (int)n_lsq.shelves.size()); ++i)
    {
      if (!n_lsq.isNew[i])
      {
        if (n_lsq.shelves[i].vw && n_lsq.shelves[i].vb && n_lsq.shelves[i].vo && n_lsq.shelves[i].va)
        {
          e = shelves[i];
          n_lsq.shelves.erase(std::begin(n_lsq.shelves) + i);
          n_lsq.isNew.erase(std::begin(n_lsq.isNew) + i);
          break;
        }
      }
    }
  }
  
  for (std::size_t i = 0; i < n_lsq.shelves.size(); ++i)
  {
    if (n_lsq.shelves[i].vw && n_lsq.shelves[i].vb && n_lsq.shelves[i].vo && !n_lsq.shelves[i].va)
    {
      shelves[i].addr = shelves[i].b + shelves[i].o;
      shelves[i].va = true;
      break;
    }
  }

  return e;
}

LSQ& LSQ::operator=(const LSQ& lsq)
{
  this->shelves = lsq.shelves;

  this->isNew = lsq.isNew;
  std::fill(std::begin(this->isNew), std::end(this->isNew), false);

  return *this;
}

std::ostream& operator<<(std::ostream& os, const LSQ& lsq)
{
  os << "    type  seq   dest  write base  off   addr  ready \n";
  os << "    ------------------------------------------------\n";
  for (std::size_t i = 0; i < lsq.shelves.size(); ++i)
  {
    os << "    ";
    os << SPACE(lsq.shelves[i].type == LSQ::Shelf::LOAD ? "L" : "S")
       << SPACE(std::to_string(lsq.shelves[i].seq))
       << SPACE(std::to_string(lsq.shelves[i].d))
       << SPACE((lsq.shelves[i].vw ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].w))
       << SPACE((lsq.shelves[i].vb ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].b))
       << SPACE((lsq.shelves[i].vo ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].o))
       << SPACE((lsq.shelves[i].va ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].addr))
       << SPACE(std::to_string(lsq.shelves[i].ready))
       << '\n';
  }
  return os;
}
