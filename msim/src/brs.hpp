#ifndef BRS_H
#define BRS_H

#include "config.hpp"

#include <string>
#include <array>
#include <tuple>
#include <vector>
#include <iostream>

// Branch Reservation Station
class BRS
{
  public:
    struct Shelf
    {
      std::string opcode = "nop";
      int o1 = 0; bool v1 = true;
      int o2 = 0; bool v2 = true;
      int o3 = 0; bool v3 = true;
      int dest = 0; 
    };

  public:
    int space() const;

    void issue(Shelf shelf);
    Shelf dispatch(BRS &n_brs, bool port);

    void update(int dest, int result);
    void reset();

    BRS& operator=(const BRS& brs);

    friend std::ostream& operator<<(std::ostream& os, const BRS& brs);

  private:
    std::vector<Shelf> shelves;
};
std::ostream& operator<<(std::ostream& os, const BRS& brs);

#endif
