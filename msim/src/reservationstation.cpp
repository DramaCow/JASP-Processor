#include "reservationstation.hpp"

std::ostream& operator<<(std::ostream& os, const ReservationStation& restat)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    os << "    " 
       << restat.entry[i].opcode << ' ' 
       << restat.entry[i].os1 << ' ' 
       << restat.entry[i].os2 << ' ' 
       << restat.entry[i].rd << '\n';
  }
  return os;
}

bool ReservationStation::insert(std::string opcode, int os1, int os2, int rd)
{
  this->entry[0].opcode = opcode;
  this->entry[0].os1 = os1;
  this->entry[0].os2 = os2;
  this->entry[0].rd = rd;
  return true;
}

Entry ReservationStation::dispatch()
{
  return entry[0];
}

ReservationStation& ReservationStation::operator=(const ReservationStation& restat)
{
  for (int i = 0; i < NUM_ENTRIES; ++i)
  {
    this->entry[i] = restat.entry[i];
  }
  return *this;
}
