#include "pt.hpp"
#include <bitset>
#include <iomanip>

#define SPACE2 std::left<<std::setfill((char)32)<<std::setw(2)<<
#define SPACE4 std::left<<std::setfill((char)32)<<std::setw(4)<<

PT::PT()
{
  std::fill(std::begin(this->entries), std::end(this->entries), 0x3);
}

bool PT::predict(int pattern)
{
  int idx = pattern % NUM_PT_ENTRIES;
  return this->entries[idx] >= 0x2; // msb is 1
}

void PT::update(int pattern, bool wasTaken)
{
  int idx = pattern % NUM_PT_ENTRIES;
  std::cout << "PATTERN: " << idx << ' ' << wasTaken << std::endl;
  int e = this->entries[idx];
  this->entries[idx] = e + (wasTaken && e < 0x3 ? 1 : (!wasTaken && e > 0x0 ? -1 : 0));
}

PT& PT::operator=(const PT& pt)
{
  this->entries = pt.entries;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const PT& pt)
{
  os << "     ";
  for (int i = 0; i < 16; ++i)
  {
    os << SPACE2(i) << ' ';
  }
  os << "\n        -------------------------------------------------\n";

  int j = 0;
  for (int i = 0; i < NUM_PT_ENTRIES; ++i)
  {
    if (i % 16 == 0)
    {
      os << "    " << SPACE4(j) << '|';
    }
    std::bitset<2> e(pt.entries[i]);
    os << e << ' ';
    if (i % 16 == 15 && i != NUM_PT_ENTRIES-1)
    {
      j += 16;
      os << '\n';
    }
  }
  return os;
}
