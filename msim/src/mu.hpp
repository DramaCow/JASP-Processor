#ifndef MU_H
#define MU_H

#include <string>
#include <vector>
#include <iostream>
#include "config.hpp"
#include "cache.hpp"

// Memory Unit
class MU
{
  public:
    MU(DCache &dcache);

    MU& operator=(const MU& mu);

    friend std::ostream& operator<<(std::ostream& os, const MU& mu);

  private:
    DCache &dcache;
};
std::ostream& operator<<(std::ostream& os, const MU& mu);

#endif
