#ifndef RAT_H
#define RAT_H

#include "config.hpp"
#include <iostream>

// Register Alias Table
class RAT
{
  public:
    RAT();

    int read(int r);
    int alloc(int r);

    friend std::ostream& operator<<(std::ostream& os, const RAT& rat);

  private:
    int table[NUM_REGISTERS];
};
std::ostream& operator<<(std::ostream& os, const RAT& rat);

#endif
