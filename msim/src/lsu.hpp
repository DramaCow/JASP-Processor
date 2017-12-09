#ifndef LSU_H
#define LSU_H

#include <string>
#include <vector>
#include <iostream>
#include "config.hpp"
#include "cache.hpp"

// Load-Store Uueue
class LSU
{
  public:
    struct Entry
    {
      enum Type { NA, LOAD, STORE };
      Type type = NA;
      int seq = 0;
      int addr = 0;
      int val = 0;
      bool fwd = false; // was val forwarded?
    };

  public:
    LSU(DCache &dcache);

    void insert(std::string opcode, int seq, int o1, int o2, int tail);
    void dispatch(Entry entry);
    void execute(LSU& n_lsu);

    int duration = 0;
    LSU::Entry next;
    bool writeback = false;

    LSU& operator=(const LSU& lsu);

    friend std::ostream& operator<<(std::ostream& os, const LSU& lsu);

  private:
    std::vector<Entry> entries;
    DCache &dcache;
};
std::ostream& operator<<(std::ostream& os, const LSU& lsu);

#endif
