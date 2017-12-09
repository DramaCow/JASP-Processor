#ifndef RS_H
#define RS_H

#include "config.hpp"

#include <string>
#include <tuple>
#include <vector>
#include <iostream>

// Reservation Station
class RS
{
  public:
    struct Shelf
    {
      std::string opcode = "nop";
//      int seq = 0; // rob entry index
      int o1 = 0; bool v1 = true;
      int o2 = 0; bool v2 = true;
      int o3 = 0; bool v3 = true;
      int dest = 0; 
    };

  public:
    bool isFull();

    void issue(Shelf shelf);
    std::tuple<Shelf, Shelf> dispatch(RS &n_rs, bool port1, bool port2);

    void update(int dest, int result);
    void reset();

    RS& operator=(const RS& rs);

    friend std::ostream& operator<<(std::ostream& os, const RS& rs);

  private:
    std::vector<Shelf> shelves;
};
std::ostream& operator<<(std::ostream& os, const RS& rs);

#endif
