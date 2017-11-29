#ifndef ROB_H
#define ROB_H

#include "config.hpp"
#include "instruction.hpp"
#include <string>
#include <iostream>
#include <tuple>
#include <vector>

// ReOrder Buffer
class ROB {
  public:
    std::tuple<int, bool> read(int addr);
    int push(ROB &n_rob, Instruction instruction, int r);
    void write(int addr, int val);
    std::vector<std::tuple<int,int,int>> pop(ROB &n_rob);

    ROB& operator=(const ROB& rob);

    friend std::ostream& operator<<(std::ostream& os, const ROB& rob);

  public:
    class ROBEntry
    {
      public:
        enum Type { DN, WB, BR }; // either do nothing, writeback, or branch

        Type type = DN;
        int reg = 0;
        int val = 0;
        bool done = false;
    };

  private:
    ROBEntry entries[NUM_ROB_ENTRIES];

    int head = 0; // issue
    int tail = 0; // commit / retire
};
std::ostream& operator<<(std::ostream& os, const ROB& rob);

#endif
