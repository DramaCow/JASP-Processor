#include "rs.hpp"

bool RS::isFull()
{
  return this->shelves.size() >= NUM_RS_ENTRIES;
}

void RS::issue(Shelf shelf)
{
  if (this->shelves.size() >= NUM_RS_ENTRIES)
  {
    return;
  }
  this->shelves.push_back(shelf);
}

std::tuple<Shelf, Shelf> RS::dispatch(RS &n_rs, bool port1, bool port2)
{
  Shelf e1;
  Shelf e2;

  std::vector<Shelf> shelves(this->shelves);

  // only looks at the entries that exist this iteration AND next iteration
  // (i.e. for length of this iteration), else we may dispatch instructions
  // that were added just this iteration - which is impossible practically.
  for (std::size_t i = 0; i < shelves.size(); ++i)
  {
    if (shelves[i].v1 && shelves[i].v2 && shelves[i].v3)
    {
      e1 = shelves[i];

      shelves.erase(std::begin(shelves) + i);
      n_rs.shelves.erase(std::begin(n_rs.shelves) + i);

      break;
    }
  }

  return std::make_tuple(e1, e2);
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

    if (!this->shelves[i].v3 && this->shelves[i].o3 == dest)
    {
      this->shelves[i].o3 = result;
      this->shelves[i].v3 = true;
    }
  }
}

RS& RS::operator=(const RS& rs)
{
  this->shelves = rs.shelves;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const RS& rs)
{
  os << "    OC  \to1  \tv1  \to2  \tv2  \to3  \tv3  \tdest\n";
  os << "    ----------------------------------------------------------------\n";
  for (std::size_t i = 0; i < rs.shelves.size(); ++i)
  {
    os << "    ";
    os << rs.shelves[i].opcode << '\t' 
       << rs.shelves[i].o1 << '\t' 
       << rs.shelves[i].v1 << '\t' 
       << rs.shelves[i].o2 << '\t' 
       << rs.shelves[i].v2 << '\t' 
       << rs.shelves[i].o3 << '\t' 
       << rs.shelves[i].v3 << '\t' 
       << rs.shelves[i].dest << '\n';
  }
  return os;
}
