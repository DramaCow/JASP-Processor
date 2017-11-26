#ifndef RAT_H
#define RAT_H

#include "config.hpp"
#include <iostream>

// Register Alias Table
class RAT
{
  public:
    RAT();

    friend std::ostream& operator<<(std::ostream& os, const RAT& rat);

  private:
    int table[NUM_REGISTERS];
};
std::ostream& operator<<(std::ostream& os, const RAT& rat);

#endif
