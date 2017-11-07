#include "reservationstation.hpp"

std::ostream& operator<<(std::ostream& os, const ReservationStation& restat)
{
  os << "    OC  \tO_s1\tV_s1\tO_s2\tV_s2\tO_s3\tV_s3\tR_d\t | \twe\tage\tfree\n";
  os << "    ------------------------------------------------------------------------------------------------\n";
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    os << "    " 
       << restat.entry[i].opcode << "  " << '\t' 
       << restat.entry[i].os1 << '\t' 
       << restat.entry[i].v1 << '\t' 
       << restat.entry[i].os2 << '\t' 
       << restat.entry[i].v2 << '\t' 
       << restat.entry[i].os3 << '\t' 
       << restat.entry[i].v3 << '\t' 
       << restat.entry[i].rd << '\t'
       << " | " << '\t'
       << restat.entry[i].we << '\t'
       << restat.entry[i].age << '\t'
       << restat.entry[i].free << '\n';
  }
  return os;
}

bool ReservationStation::isFull()
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    if (this->entry[i].free)
    {
      return false;
    }
  }
  return true;
}

void ReservationStation::issue(Entry entry)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
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

Entry ReservationStation::dispatch(ReservationStation &n_restat)
{
  int oldest = -1;
  for (int i = 0; i < NUM_ENTRIES; ++i)
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
    n_restat.entry[oldest].free = true;
    return this->entry[oldest];
  }
}

void ReservationStation::update(int result, int rd)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
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

void ReservationStation::tick()
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      this->entry[i].age++;
    }
  }
}

ReservationStation& ReservationStation::operator=(const ReservationStation& restat)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    this->entry[i] = restat.entry[i];
  }
  return *this;
}
