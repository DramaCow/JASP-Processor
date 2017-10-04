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
    case ADD:
      res = areg + breg;
      break;

    case SUB:
      res = areg - breg;
      break;

    default:
      // TODO: error message
      std::cerr << "*** invalid ALU op ***\n";
      exit(EXIT_FAILURE);
  }
}
