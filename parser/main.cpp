#include <iostream>
#include "parser.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "usage: ./parser <program_name>" << std::endl;
    exit(EXIT_FAILURE);
  }

  Parser parser;
  parser.load_program(argv[1]);

  return 0;
}
