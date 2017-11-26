#ifndef RAT_H
#define RAT_H

#include "config.hpp"
#include "rob.hpp"
#include "rrf.hpp"
#include <iostream>

// Register Alias Table
class RAT
{
  public:
    RAT(ROB rob, RRF rrf);

    int read(int a);
    int alloc(int a);

    friend std::ostream& operator<<(std::ostream& os, const RAT& rat);

  private:
    int table[NUM_REGISTERS];

    ROB &rob;
    RRF &rrf;
};
std::ostream& operator<<(std::ostream& os, const RAT& rat);

#endif
