#include "lsq.hpp"
#include <iomanip>
#include <algorithm>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

bool LSQ::isFull()
{
  return this->shelves.size() >= NUM_LSQ_ENTRIES;
}

void LSQ::issue(Shelf shelf, int tail)
{
  std::cout << " ---- INSERTED ----\n";

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
  this->isNew.insert(std::begin(this->isNew) + i, true);
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
        if (n_lsq.shelves[i].vw && n_lsq.shelves[i].vb && n_lsq.shelves[i].vo && n_lsq.shelves[i].va && n_lsq.shelves[i].ready)
        {
          e = shelves[i];
//        n_lsq.shelves.erase(std::begin(n_lsq.shelves) + i);
//        n_lsq.isNew.erase(std::begin(n_lsq.isNew) + i);
          break;
        }
      }
    }
  }
  
  for (std::size_t i = 0; i < n_lsq.shelves.size(); ++i)
  {
    if (!n_lsq.isNew[i])
    {
      if (n_lsq.shelves[i].vw && n_lsq.shelves[i].vb && n_lsq.shelves[i].vo && !n_lsq.shelves[i].va)
      {
        n_lsq.shelves[i].addr = shelves[i].b + shelves[i].o;
        n_lsq.shelves[i].va = true;
        if (n_lsq.shelves[i].type == LSQ::Shelf::LOAD)
        {
          n_lsq.shelves[i].ready = true;
        }
        break;
      }
    }
  }

  std::fill(std::begin(n_lsq.isNew), std::end(n_lsq.isNew), false);

  return e;
}

void LSQ::retire(int seq)
{
  for (std::size_t i = 0; i < this->shelves.size(); ++i)
  {
    if (this->shelves[i].seq == seq)
    {
      this->shelves.erase(std::begin(this->shelves) + i);
      this->isNew.erase(std::begin(this->isNew) + i);
      return;
    }
  }
}

void LSQ::update(int dest, int result)
{
  for (std::size_t i = 0; i < this->shelves.size(); ++i)
  {
    if (!this->shelves[i].vw && this->shelves[i].w == dest)
    {
      this->shelves[i].w = result;
      this->shelves[i].vw = true;
    }

    if (!this->shelves[i].vb && this->shelves[i].b == dest)
    {
      this->shelves[i].b = result;
      this->shelves[i].vb = true;
    }

    if (!this->shelves[i].vo && this->shelves[i].o == dest)
    {
      this->shelves[i].o = result;
      this->shelves[i].vo = true;
    }
  }
}

void LSQ::reset()
{
  this->shelves.clear();
  this->isNew.clear();
}

LSQ& LSQ::operator=(const LSQ& lsq)
{
  this->shelves = lsq.shelves;
  this->isNew = lsq.isNew;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const LSQ& lsq)
{
  os << "    type  seq   dest  write base  off   addr  ready \n";
  os << "    ------------------------------------------------\n";
  for (std::size_t i = 0; i < lsq.shelves.size(); ++i)
  {
    os << "    ";
    os << SPACE(lsq.shelves[i].type == LSQ::Shelf::LOAD ? "L" : lsq.shelves[i].type == LSQ::Shelf::STORE ? "S" : "--")
       << SPACE(std::to_string(lsq.shelves[i].seq))
       << SPACE(std::to_string(lsq.shelves[i].d))
       << SPACE((lsq.shelves[i].vw ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].w))
       << SPACE((lsq.shelves[i].vb ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].b))
       << SPACE((lsq.shelves[i].vo ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].o))
       << SPACE((lsq.shelves[i].va ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].addr))
       << SPACE(std::to_string(lsq.shelves[i].ready))
       << SPACE(lsq.isNew[i] ? "NEW" : "")
       << '\n';
  }
  return os;
}
