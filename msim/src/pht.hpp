#ifndef PHT_H
#define PHT_H

#include "config.hpp"
#include <array>
#include <iostream>
#include <cstdint>

// Branch History Table
class PHT
{
  public:
    PHT();
    bool predict(int pc);
    void update(int pc, bool wasTaken);

    PHT& operator=(const PHT& pht);

    friend std::ostream& operator<<(std::ostream& os, const PHT& pht);
    
  private:
    std::array<uint32_t,NUM_PHT_ENTRIES> entries;
};
std::ostream& operator<<(std::ostream& os, const PHT& pht);

#endif
