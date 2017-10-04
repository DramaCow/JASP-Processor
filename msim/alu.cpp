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
    // ADD
    case 0:
      res = areg + breg;
      break;

    // SUB
    case 1:
      res = areg - breg;
      break;

    default:
      // TODO: error message
      std::cerr << "*** invalid ALU op ***\n";
      exit(EXIT_FAILURE);
  }
}
