#include "hrt.hpp"
#include <bitset>
#include <iomanip>

#define SPACEH std::left<<std::setfill((char)32)<<std::setw(HREG_BITS)<<
#define SPACE4 std::left<<std::setfill((char)32)<<std::setw(4)<<

HRT::HRT()
{
  std::fill(std::begin(this->entries), std::end(this->entries), 0x0);
}

int HRT::history(int pc)
{
  int idx = pc % NUM_HRT_ENTRIES;
  return this->entries[idx];
}

void HRT::update(int pc, bool wasTaken)
{
  int idx = pc % NUM_HRT_ENTRIES;
  int e = this->entries[idx];
  this->entries[idx] = ((e << 1) | wasTaken) % (1 << HREG_BITS);
}

std::ostream& operator<<(std::ostream& os, const HRT& hrt)
{
  os << "     ";
  for (int i = 0; i < 16; ++i)
  {
    os << SPACEH(i) << ' ';
  }
  os << "\n        --------------------------------------------------------------------------------\n";

  int j = 0;
  for (int i = 0; i < NUM_HRT_ENTRIES; ++i)
  {
    if (i % 16 == 0)
    {
      os << "    " << SPACE4(j) << '|';
    }
    std::bitset<HREG_BITS> e(hrt.entries[i]);
    os << e << ' ';
    if (i % 16 == 15 && i != NUM_HRT_ENTRIES-1)
    {
      j += 16;
      os << '\n';
    }
  }
  return os;
}
