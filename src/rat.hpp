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
    void write(RAT &n_rat, int r, int a);
    void reset();

    RAT& operator=(const RAT& rat);

    friend std::ostream& operator<<(std::ostream& os, const RAT& rat);

  private:
    int table[NUM_REGISTERS];
};
std::ostream& operator<<(std::ostream& os, const RAT& rat);

#endif
