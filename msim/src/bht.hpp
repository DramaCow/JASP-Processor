#ifndef BHT_H
#define BHT_H

#include "config.hpp"
#include <array>
#include <iostream>
#include <cstdint>

// Branch History Table
class BHT
{
  public:
    BHT();
    bool predict(int pc);
    void update(int pc, bool wasTaken);

    BHT& operator=(const BHT& bht);

    friend std::ostream& operator<<(std::ostream& os, const BHT& bht);
    
  private:
    std::array<uint32_t,NUM_BHT_ENTRIES> entries;
};
std::ostream& operator<<(std::ostream& os, const BHT& bht);

#endif
