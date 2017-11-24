#include "rs.hpp"

std::ostream& operator<<(std::ostream& os, const RS& rs)
{
  os << "    OC  \tO_s1\tV_s1\tO_s2\tV_s2\tO_s3\tV_s3\tR_d\n";
  os << "    ---------------------------------------------------------------\n";
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    os << "    ";
    if (rs.entry[i].free)
    {
      os << '*';
    }
    os << rs.entry[i].opcode << '(' << rs.entry[i].age << ')' << '\t' 
       << rs.entry[i].os1 << '\t' 
       << rs.entry[i].v1 << '\t' 
       << rs.entry[i].os2 << '\t' 
       << rs.entry[i].v2 << '\t' 
       << rs.entry[i].os3 << '\t' 
       << rs.entry[i].v3 << '\t' 
       << rs.entry[i].rd << '\n';
  }
  return os;
}

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

void RS::update(int result, int rd)
{
  for (int i = 0; i < NUM_RS_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      if (!this->entry[i].v1 && this->entry[i].os1 == rd)
      {
        this->entry[i].v1 = true;
        this->entry[i].os1 = result;
      }

      if (!this->entry[i].v2 && this->entry[i].os2 == rd)
      {
        this->entry[i].v2 = true;
        this->entry[i].os2 = result;
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
