#include "alu.hpp"
#include <cstdlib>

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
      exit(EXIT_FAILURE);
  }
}
