#include "bp.hpp"

std::tuple<int, bool> BP::predict(Instruction instruction, int pc)
{
  // i.e. assume we didn't just take the branch
  return std::make_tuple(pc + 1, false);
}
