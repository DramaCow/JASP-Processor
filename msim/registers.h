#ifndef REGISTERS_H
#define REGISTERS_H

class Registers
{
  public:

  private:
    int gpr[32]; // General Purpose Registers
    int pc, npc; // (Next) Program Counter
    int sp;      // Stack Pointer
    int fp;      // Frame Pointer
    int lr;      // Link Register
};

#endif
