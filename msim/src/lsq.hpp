#ifndef LSQ_H
#define LSQ_H

#include <string>
#include <vector>
#include "config.hpp"

// Load-Store Queue
class LSQ
{
  public:
    struct Entry
    {
      enum Type { LOAD, STORE };
      Type type;
      int seq = 0;
      int addr = 0;
      int val = 0;
    };

  public:
    void insert(Entry entry, int tail);
    void dispatch(Entry entry);

    friend std::ostream& operator<<(std::ostream& os, const LSQ& lsq);

  private:
    std::vector<Entry> entries;
};
std::ostream& operator<<(std::ostream& os, const LSQ& lsq);

#endif
