#ifndef BP_H
#define BP_H

#include "instruction.hpp"
#include <tuple>

// Branch Predictor
class BP
{
  public:
    std::tuple<int, bool> predict(Instruction instruction, int pc);
};

#endif
