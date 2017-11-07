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
  int os3 = 0; bool v3 = true;
  int rd = 0; 

  // "meta"-data
  bool we = false;
  int age = 0;
  bool free = true;
};

class ReservationStation
{
  public:
    friend std::ostream& operator<<(std::ostream& os, const ReservationStation& restat);
    bool isFull();
    void insert(Entry entry);
    Entry dispatch(ReservationStation &n_restat);
    void update(int result, int rd);
    void tick();
    ReservationStation& operator=(const ReservationStation& restat);

  private:
    Entry entry[NUM_ENTRIES];
};
std::ostream& operator<<(std::ostream& os, const ReservationStation& restat);

#endif
