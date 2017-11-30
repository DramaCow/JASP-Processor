#include "bp.hpp"

std::tuple<int, bool> BP::predict(Instruction instruction, int pc)
{
  int target = pc + 1;

  if (instruction.opcode == "b")
  {
    target = instruction.params[0];
  }
  else if (instruction.opcode == "beq"  ||
           instruction.opcode == "bneq"    )
  {
    target = instruction.params[2];
  }

  return std::make_tuple(target, true); // alway branch
  //return std::make_tuple(pc+1, false); // always not branch
}
