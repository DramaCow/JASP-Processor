#ifndef ROB_H
#define ROB_H

#include "config.hpp"
#include <iostream>

// ReOrder Buffer
class ROB {
  public:
    int alloc(int r);

    friend std::ostream& operator<<(std::ostream& os, const ROB& rob);

  private:
    int reg[NUM_ROB_ENTRIES] = { 0 };
    int val[NUM_ROB_ENTRIES] = { 0 };
    bool done[NUM_ROB_ENTRIES] = { false };

    int head = 0; // issue
    int tail = 0; // commit / retire
};
std::ostream& operator<<(std::ostream& os, const ROB& rob);

#endif
