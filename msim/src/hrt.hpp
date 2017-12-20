#ifndef HRT_H
#define HRT_H

#include "config.hpp"
#include <iostream>
#include <array>

// History Register Table
class HRT
{
  public:
    HRT();
    void update(int pc, bool wasTaken);

    friend std::ostream& operator<<(std::ostream& os, const HRT& hrt);
    
  private:
    std::array<uint32_t,NUM_HRT_ENTRIES> entries;
};
std::ostream& operator<<(std::ostream& os, const HRT& hrt);

#endif
