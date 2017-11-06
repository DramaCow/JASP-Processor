#include "reservationstation.hpp"

std::ostream& operator<<(std::ostream& os, const ReservationStation& restat)
{
  os << "    OC  \tO_s1\tV_s1\tO_s2\tV_s2\tR_d\tage\tfree\n";
  os << "    ----------------------------------------------------------------\n";
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    os << "    " 
       << restat.entry[i].opcode << "  " << '\t' 
       << restat.entry[i].os1 << '\t' 
       << restat.entry[i].v1 << '\t' 
       << restat.entry[i].os2 << '\t' 
       << restat.entry[i].v2 << '\t' 
       << restat.entry[i].rd << '\t'
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

void ReservationStation::insert(Entry entry)
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
  int size = 0;
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      if (this->entry[i].v1 && this->entry[i].v2)
      {
        oldest = (oldest == -1) ? i : (this->entry[i].age > this->entry[oldest].age) ? i : oldest;
      }
      size++;
    }
  }

  if (oldest == -1)
  {
    Entry entry; // default nop entry
    entry.opcode = "stall";
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
  // dependency index
  bool d = -1; 

  // find oldest entry dependent on rd
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      if ( (!this->entry[i].v1 && this->entry[i].os1 == rd) ||
           (!this->entry[i].v2 && this->entry[i].os2 == rd)    )
      {
        d = (d == -1) ? i : (this->entry[i].age > this->entry[d].age) ? i : d;
      }
    }
  }

  // replace operands with oldest
  if (d != -1)
  {
    // update the oldest entry
    if (!this->entry[d].v1 && this->entry[d].os1 == rd)
    {
      this->entry[d].v1 = true; this->entry[d].os1 = result;
    }
    if (!this->entry[d].v2 && this->entry[d].os2 == rd)
    {
      this->entry[d].v2 = true; this->entry[d].os2 = result;
    }

    // if oldest entry does not modify the rd
    if (this->entry[d].rd != rd)
    {
      // repeat for next oldest entry
      this->update(result, rd);
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
