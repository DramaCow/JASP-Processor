#include "alu.hpp"
#include <cstdlib>
#include <iostream>

Alu::Alu()
{
}

void Alu::execute()
{
  switch (op)
  {
    case OP_ADD: {
      res = areg + breg;
      break;
    }

    // SUB
    case OP_SUB: {
      res = areg - breg;
      break;
    }

    case OP_XOR: {
      res = areg ^ breg;
      break;
    }

    default: {
      // TODO: error message
      std::cerr << "*** invalid ALU op ***\n";
      exit(EXIT_FAILURE);
    }
  }
}
