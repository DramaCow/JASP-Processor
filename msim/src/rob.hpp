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
        int pc = -1;
        int reg = -1;
        int val = 0;
        bool pred = false;
        bool taken = false;
        bool spec = false;
        bool done = false;

        Instruction instruction;
    };

  public:
    int space() const;

    int push(ROB &n_rob, int pc, Instruction instruction, int reg, bool pred);
    std::vector<std::tuple<int,ROB::Entry>> pop(ROB &n_rob, LSQ &n_lsq);

    std::tuple<int, bool> read(int addr);
    void write(int addr, int val);
    void write(int addr, int val, bool t);

    void handle_store_to_load_forwarding();
    void reset();
    void set_spec(int seq, bool spec);

    int get_tail();
    int size = 0;

    ROB& operator=(const ROB& rob);

    friend std::ostream& operator<<(std::ostream& os, const ROB& rob);

  private:
    std::array<Entry,NUM_ROB_ENTRIES> entries;

    int head = 0; // issue
    int tail = 0; // commit / retire
};
std::ostream& operator<<(std::ostream& os, const ROB& rob);

#endif
