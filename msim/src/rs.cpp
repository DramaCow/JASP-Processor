#include "rs.hpp"

bool RS::isFull()
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (this->entry[i].free)
    {
      return false;
    }
  }
  return true;
}

void RS::issue(Entry entry)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (this->entry[i].free)
    {
      this->entry[i] = entry;
      this->entry[i].age = 0;
      this->entry[i].free = false;

      return;
    }
  }
}

Entry RS::dispatch(RS &n_rs)
{
  int oldest = -1;
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      if (this->entry[i].v1 && this->entry[i].v2)
      {
        oldest = (oldest == -1) ? i : (this->entry[i].age > this->entry[oldest].age) ? i : oldest;
      }
    }
  }

  if (oldest == -1)
  {
    Entry entry; // default nop entry
    return entry;
  }
  else
  {
    n_rs.entry[oldest].free = true;
    return this->entry[oldest];
  }
}

void RS::update(int result, int dest)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      if (!this->entry[i].v1 && this->entry[i].o1 == dest)
      {
        this->entry[i].v1 = true;
        this->entry[i].o1 = result;
      }

      if (!this->entry[i].v2 && this->entry[i].o2 == dest)
      {
        this->entry[i].v2 = true;
        this->entry[i].o2 = result;
      }
    }
  }
}

void RS::tick()
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      this->entry[i].age++;
    }
  }
}

RS& RS::operator=(const RS& rs)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    this->entry[i] = rs.entry[i];
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
    if (rs.entry[i].free)
    {
      os << '*';
    }
    os << rs.entry[i].opcode << '(' << rs.entry[i].age << ')' << '\t' 
       << rs.entry[i].o1 << '\t' 
       << rs.entry[i].v1 << '\t' 
       << rs.entry[i].o2 << '\t' 
       << rs.entry[i].v2 << '\t' 
       << rs.entry[i].o3 << '\t' 
       << rs.entry[i].v3 << '\t' 
       << rs.entry[i].dest << '\n';
  }
  return os;
}
