#ifndef LSU_H
#define LSU_H

#include <string>
#include <vector>
#include <iostream>
#include "config.hpp"

// Load-Store Uueue
class LSU
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
    void insert(std::string opcode, int seq, int o1, int o2, int tail);
    void dispatch(Entry entry);

    friend std::ostream& operator<<(std::ostream& os, const LSU& lsu);

  private:
    std::vector<Entry> entries;
};
std::ostream& operator<<(std::ostream& os, const LSU& lsu);

#endif
