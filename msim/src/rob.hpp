#ifndef ROB_H
#define ROB_H

#include "config.hpp"
#include "instruction.hpp"
#include "lsq.hpp"
#include <string>
#include <iostream>
#include <tuple>
#include <vector>

// ReOrder Buffer
class ROB {
  public:
    class Entry
    {
      public:
        enum Type { DN, WB, BR, SR, END };

        Type type = DN;
        int reg = -1;
        int val = 0;
        int target = -1;
        bool spec = false;
        bool done = false;
    };

  public:
    int push(ROB &n_rob, std::string opcode, int r, int target);
    std::vector<std::tuple<int,ROB::Entry>> pop(ROB &n_rob, LSQ &n_lsq);

    std::tuple<int, bool> read(int addr);
    void write(int addr, int val);
    void write(int addr, int val, int target);

    void handle_store_to_load_forwarding();
    void reset();

    int get_tail();

    ROB& operator=(const ROB& rob);

    friend std::ostream& operator<<(std::ostream& os, const ROB& rob);

  private:
    Entry entries[NUM_ROB_ENTRIES];

    int head = 0; // issue
    int tail = 0; // commit / retire
};
std::ostream& operator<<(std::ostream& os, const ROB& rob);

#endif
