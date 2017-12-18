#ifndef RRF_H
#define RRF_H

#include "config.hpp"

#include <cstdint>
#include <tuple>
#include <iostream>

// Retirement Register File (i.e. the program visible state)
class RRF
{
  public:
    RRF();

    std::tuple<int, bool> read(int r);
    void reset(int r);
    void write(int r, int data);

    bool operator==(const RRF& rrf);
    RRF& operator=(const RRF& rrf);

    friend std::ostream& operator<<(std::ostream& os, const RRF& rrf);

  private:
    uint32_t gpr[NUM_REGISTERS] = { 0 };
    bool v[NUM_REGISTERS]; // valid bit (scoreboarding)
};
std::ostream& operator<<(std::ostream& os, const RRF& rrf);

#endif
