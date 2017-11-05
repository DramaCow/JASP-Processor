#ifndef RESERVATIONSTATION_H
#define RESERVATIONSTATION_H

#define NUM_ENTRIES 4

#include <string>
#include <iostream>

struct Entry
{
  std::string opcode = "nop";
  int os1 = 0; bool v1 = true;
  int os2 = 0; bool v2 = true;
  int rd = 0;

  int age = 0;
  bool free = true;
};

class ReservationStation
{
  public:
    friend std::ostream& operator<<(std::ostream& os, const ReservationStation& restat);
    bool insert(std::string opcode, int os1, int os2, int rd);
    Entry dispatch(ReservationStation &n_restat);
    ReservationStation& operator=(const ReservationStation& restat);

  private:
    Entry entry[NUM_ENTRIES];
};
std::ostream& operator<<(std::ostream& os, const ReservationStation& restat);

#endif
