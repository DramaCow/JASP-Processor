#ifndef RRF_H
#define RRF_H

#include <cstdint>
#include <tuple>
#include <iostream>

#define NUM_REGISTERS 8

// Retirement Register File (i.e. the program visible state)
class RRF
{
  public:
    RRF();
    std::tuple<uint32_t, uint32_t> foo(uint32_t addr1, uint32_t addr2, uint32_t daddr, uint32_t data, bool we);

    std::tuple<int, bool> read(int addr);
    void reset(int addr);
    void write(int addr, int data);

    friend std::ostream& operator<<(std::ostream& os, const RRF& rrf);
    RRF& operator=(const RRF& rrf);

  private:
    uint32_t gpr[NUM_REGISTERS] = { 0 };
    bool v[NUM_REGISTERS]; // valid bit (scoreboarding)
};

std::ostream& operator<<(std::ostream& os, const RRF& rrf);

#endif
