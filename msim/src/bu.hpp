#ifndef BU_H
#define BU_H

#include "brs.hpp"
#include <string>
#include <iostream>

// Branching Unit
class BU
{
  public:
    void dispatch(BRS::Shelf shelf);

    void reset();

    // input latch
    BRS::Shelf shelf;

    // output latch
    int result = false;

    // control flags
    bool writeback = false;

    friend std::ostream& operator<<(std::ostream& os, const BU& bu);
};
std::ostream& operator<<(std::ostream& os, const BU& bu);

#endif
