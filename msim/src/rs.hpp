#ifndef RS_H
#define RS_H

#include "config.hpp"

#include <string>
#include <tuple>
#include <iostream>

struct Shelf
{
  std::string opcode = "nop";
  int o1 = 0; bool v1 = true;
  int o2 = 0; bool v2 = true;
  int o3 = 0; bool v3 = true;
  int dest = 0; 

  // "meta"-data
  int age = 0;
  bool free = true;
};

// Reservation Station
class RS
{
  public:
    bool isFull();
    void issue(Shelf shelf);
    std::tuple<Shelf, Shelf> dispatch(RS &n_rs, bool port1, bool port2);
    void update(int dest, int result);
    void tick();

    RS& operator=(const RS& rs);

    friend std::ostream& operator<<(std::ostream& os, const RS& rs);

  private:
    Shelf shelf[NUM_RS_ENTRIES];
};
std::ostream& operator<<(std::ostream& os, const RS& rs);

#endif
