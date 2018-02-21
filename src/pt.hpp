#ifndef PT_H
#define PT_H

#include "config.hpp"
#include <array>
#include <iostream>
#include <cstdint>

// Pattern Table
class PT
{
  public:
    PT();
    bool predict(int pattern);
    void update(int pattern, bool wasTaken);

    PT& operator=(const PT& pt);

    friend std::ostream& operator<<(std::ostream& os, const PT& pt);
    
  private:
    std::array<uint32_t,NUM_PT_ENTRIES> entries;
};
std::ostream& operator<<(std::ostream& os, const PT& pt);

#endif
