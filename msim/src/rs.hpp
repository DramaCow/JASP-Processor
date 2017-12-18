#ifndef RS_H
#define RS_H

#include "config.hpp"

#include <string>
#include <array>
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
      int o1 = 0; bool v1 = true;
      int o2 = 0; bool v2 = true;
      int dest = 0; 
    };

  public:
    int space() const;

    void issue(Shelf shelf);
    std::array<Shelf,NUM_ALUS> dispatch(RS &n_rs, std::array<bool,NUM_ALUS> port);

    void update(int dest, int result);
    void reset();

    RS& operator=(const RS& rs);

    friend std::ostream& operator<<(std::ostream& os, const RS& rs);

  private:
    std::vector<Shelf> shelves;
};
std::ostream& operator<<(std::ostream& os, const RS& rs);

#endif
