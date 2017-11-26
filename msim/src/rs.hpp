#ifndef RS_H
#define RS_H

#include "config.hpp"

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
  int age = 0;
  bool free = true;
};

// Reservation Station
class RS
{
  public:
    bool isFull();
    void issue(Entry entry);
    Entry dispatch(RS &n_rs);
    void update(int result, int rd);
    void tick();

    RS& operator=(const RS& rs);

    friend std::ostream& operator<<(std::ostream& os, const RS& rs);

  private:
    Entry entry[NUM_RS_ENTRIES];
};
std::ostream& operator<<(std::ostream& os, const RS& rs);

#endif
