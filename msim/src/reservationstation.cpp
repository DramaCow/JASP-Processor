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

bool ReservationStation::insert(std::string opcode, int os1, int os2, int rd)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    if (this->entry[i].free)
    {
      this->entry[i].opcode = opcode;
      this->entry[i].os1 = os1;
      this->entry[i].os2 = os2;
      this->entry[i].rd = rd;

      this->entry[i].age = 0;
      this->entry[i].free = false;

      return true;
    }
  }
  return false;
}

Entry ReservationStation::dispatch(ReservationStation &n_restat)
{
  int oldest = -1;
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    if (!this->entry[i].free)
    {
      oldest = (oldest == -1) ? i : (this->entry[i].age > this->entry[oldest].age) ? i : oldest;
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

ReservationStation& ReservationStation::operator=(const ReservationStation& restat)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    this->entry[i] = restat.entry[i];
  }
  return *this;
}
