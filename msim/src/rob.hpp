#ifndef ROB_H
#define ROB_H

// ReOrder Buffer
class ROB {
  private:
    int reg[NUM_ROB_ENTRIES];
    int val[NUM_ROB_ENTRIES];
    int done[NUM_ROB_ENTRIES];

    int head = 0; // issue
    int tail = 0; // commit / retire
};

#endif
