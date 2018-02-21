#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <vector>
#include <iostream>

class Instruction
{
  public:
    Instruction();

    std::string opcode = "nop";
    std::vector<int> params;

    // "hidden" members - used for meta stuff
    // (would be part of ibuf if I wasn't lazy)
    int npc;
    int pattern;
    bool pred;

    int getTakenBTA();

    static bool isArth(std::string opcode); // is logical/arithmetic instruction
    static bool isBrch(std::string opcode); // is branch instruction
    static bool isCondBrch(std::string opcode); // is conditional branch instruction
    static bool isLdsr(std::string opcode); // is load/store instruction

    friend std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);
};
std::ostream& operator<<(std::ostream& os, const Instruction& Instruction);

#endif
