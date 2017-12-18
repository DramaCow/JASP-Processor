#include "bht.hpp"

BHT::BHT()
{
  std::fill(std::begin(this->entries), std::end(this->entries), 0x3);
}

bool BHT::predict(int pc)
{
  int idx = pc % NUM_BHT_ENTRIES;
  return this->entries[idx] >= 0x2; // msb is 1
}

void BHT::update(int pc, bool wasTaken)
{
  int idx = pc % NUM_BHT_ENTRIES;
  int e = this->entries[idx];
  this->entries[idx] = e + (wasTaken && e < 0x3 ? 1 : (!wasTaken && e > 0x0 ? -1 : 0));
}

std::ostream& operator<<(std::ostream& os, const BHT& bht)
{
  for (int i = 0; i < NUM_BHT_ENTRIES; ++i)
  {
    os << bht.entries[i] << ' ';
    if (i % 64 == 63 && i != NUM_BHT_ENTRIES-1)
    {
      os << "\n    ";
    }
  }
  return os;
}
