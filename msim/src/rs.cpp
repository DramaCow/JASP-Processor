#include "rs.hpp"

bool RS::isFull()
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (this->shelf[i].free)
    {
      return false;
    }
  }
  return true;
}

void RS::issue(Shelf shelf)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (this->shelf[i].free)
    {
      this->shelf[i] = shelf;
      this->shelf[i].age = 0;
      this->shelf[i].free = false;

      return;
    }
  }
}

Shelf RS::dispatch(RS &n_rs)
{
  int oldest = -1;
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->shelf[i].free)
    {
      if (this->shelf[i].v1 && this->shelf[i].v2)
      {
        oldest = (oldest == -1) ? i : (this->shelf[i].age > this->shelf[oldest].age) ? i : oldest;
      }
    }
  }

  if (oldest == -1)
  {
    Shelf shelf; // default nop shelf
    return shelf;
  }
  else
  {
    n_rs.shelf[oldest].free = true;
    return this->shelf[oldest];
  }
}

void RS::update(int result, int dest)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->shelf[i].free)
    {
      if (!this->shelf[i].v1 && this->shelf[i].o1 == dest)
      {
        this->shelf[i].v1 = true;
        this->shelf[i].o1 = result;
      }

      if (!this->shelf[i].v2 && this->shelf[i].o2 == dest)
      {
        this->shelf[i].v2 = true;
        this->shelf[i].o2 = result;
      }
    }
  }
}

void RS::tick()
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->shelf[i].free)
    {
      this->shelf[i].age++;
    }
  }
}

RS& RS::operator=(const RS& rs)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    this->shelf[i] = rs.shelf[i];
  }
  return *this;
}

std::ostream& operator<<(std::ostream& os, const RS& rs)
{
  os << "    OC  \to1  \tv1  \to2  \tv2  \to3  \tv3  \tdest\n";
  os << "    ---------------------------------------------------------------\n";
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    os << "    ";
    if (rs.shelf[i].free)
    {
      os << '*';
    }
    os << rs.shelf[i].opcode << '(' << rs.shelf[i].age << ')' << '\t' 
       << rs.shelf[i].o1 << '\t' 
       << rs.shelf[i].v1 << '\t' 
       << rs.shelf[i].o2 << '\t' 
       << rs.shelf[i].v2 << '\t' 
       << rs.shelf[i].o3 << '\t' 
       << rs.shelf[i].v3 << '\t' 
       << rs.shelf[i].dest << '\n';
  }
  return os;
}
