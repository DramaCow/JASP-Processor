#include "lsq.hpp"
#include <iomanip>
#include <algorithm>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

int LSQ::space() const
{
  //return this->shelves.size() >= NUM_LSQ_ENTRIES;
  //return (NUM_LSQ_ENTRIES - this->shelves.size()) < FETCHRATE;
  return NUM_LSQ_ENTRIES - this->shelves.size();
}

void LSQ::issue(Shelf shelf, int tail)
{
  std::size_t i;
  for (i = 0; i < this->shelves.size(); ++i)
  {
    int s1 = shelf.seq-NUM_REGISTERS + (shelf.seq-NUM_REGISTERS < tail ? NUM_ROB_ENTRIES : 0);
    int s2 = this->shelves[i].seq-NUM_REGISTERS + (this->shelves[i].seq-NUM_REGISTERS < tail ? NUM_ROB_ENTRIES : 0);

    if (s1 < s2)
    {
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
    for (std::size_t i = 0; i < n_lsq.shelves.size(); ++i)
    //for (int i = 0; i < std::min(1, (int)n_lsq.shelves.size()); ++i)
    {
      if (!n_lsq.isNew[i] && ((i == 0) || (i > 0 && n_lsq.shelves[i].type != LSQ::Shelf::STORE)))
      {
        if (n_lsq.shelves[i].vw && n_lsq.shelves[i].vb && n_lsq.shelves[i].vo && n_lsq.shelves[i].va && n_lsq.shelves[i].ready)
        {
          n_lsq.store_to_load_forward(i);
          e = n_lsq.shelves[i];
          break;
        }
      }
    }
  }
  
  for (std::size_t i = 0; i < n_lsq.shelves.size(); ++i)
  {
    if (!n_lsq.isNew[i])
    {
      if (n_lsq.shelves[i].vb && n_lsq.shelves[i].vo && !n_lsq.shelves[i].va)
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

  // meta - this is the last time it is used, thus needs to be reset
  std::fill(std::begin(n_lsq.isNew), std::end(n_lsq.isNew), false);

  return e;
}

void LSQ::retire(int seq)
{
  int i = this->get_shelf(seq);
  if (i == -1) return;
  this->shelves.erase(std::begin(this->shelves) + i);
  this->isNew.erase(std::begin(this->isNew) + i);
}

void LSQ::mark(int seq)
{
  int i = this->get_shelf(seq);
  if (i == -1) return;
  this->shelves[i].ready = true;
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

int LSQ::get_shelf(int seq)
{
  for (std::size_t i = 0; i < this->shelves.size(); ++i)
  {
    if (this->shelves[i].seq == seq)
    {
      return i;
    }
  }
  return -1;
}

void LSQ::store_to_load_forward(int i)
{
  if (this->shelves[i].type == LSQ::Shelf::LOAD)
  {
    for (int j = i-1; j >= 0; --j)
    {
      if (!this->isNew[j])
      {
        if (this->shelves[j].type == LSQ::Shelf::STORE)
        {
          if (this->shelves[j].va && this->shelves[j].addr == this->shelves[i].addr)
          {
            this->shelves[i].w = this->shelves[j].w;
            this->shelves[i].fwd = true;
          }
          return;
        }
      }
    }
  }
  else if (this->shelves[i].type == LSQ::Shelf::STORE)
  {
    for (std::size_t j = i+1; j < this->shelves.size(); ++j)
    {
      if (!this->isNew[j])
      {
        if (
          this->shelves[j].type == LSQ::Shelf::LOAD && 
          this->shelves[j].va && this->shelves[j].addr == this->shelves[i].addr
        )
        {
          this->shelves[j].w = this->shelves[i].w;
          this->shelves[j].fwd = true;
        }
        else if (this->shelves[j].type == LSQ::Shelf::STORE && 
          (!this->shelves[j].va || 
          (this->shelves[j].va && this->shelves[j].addr == this->shelves[i].addr))
        )
        {
          return;
        }
      }
    }
  }
}

LSQ& LSQ::operator=(const LSQ& lsq)
{
  this->shelves = lsq.shelves;
  this->isNew = lsq.isNew;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const LSQ& lsq)
{
  os << "    space=" << lsq.space() << '\n';
  os << "    type  seq   dest  write base  off   addr  \n";
  os << "    ------------------------------------------\n";
  for (std::size_t i = 0; i < lsq.shelves.size(); ++i)
  {
    os << "    ";
    os << SPACE(
            (lsq.shelves[i].ready ? std::string("*") : std::string("")) + 
            (lsq.shelves[i].type == LSQ::Shelf::LOAD ? "L" : lsq.shelves[i].type == LSQ::Shelf::STORE ? "S" : "--")
          )
       << SPACE(std::string("d") + std::to_string(lsq.shelves[i].seq));

    if (lsq.shelves[i].type == LSQ::Shelf::LOAD)
    {
       os << SPACE(std::string("d") + std::to_string(lsq.shelves[i].d));
    }
    else
    {
      os << SPACE("--");
    }

    if (lsq.shelves[i].type == LSQ::Shelf::STORE || (lsq.shelves[i].type == LSQ::Shelf::LOAD && lsq.shelves[i].fwd))
    {
      os << SPACE(
              (lsq.shelves[i].vw ? std::string("") : std::string("d")) + 
              std::to_string(lsq.shelves[i].w) 
            );
    }
    else
    {
      os << SPACE("--");
    }

    os << SPACE((lsq.shelves[i].vb ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].b))
       << SPACE((lsq.shelves[i].vo ? std::string("") : std::string("d")) + std::to_string(lsq.shelves[i].o));

    if (lsq.shelves[i].va)
    {
      os << SPACE(std::string("[") + std::to_string(lsq.shelves[i].addr) + std::string("]"));
    }
    else
    {
      os << SPACE("??");
    }

    os << '\n';
  }
  return os;
}
