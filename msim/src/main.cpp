#include <iostream>
#include <cstdio>

//#include "loader.hpp"
#include "parser.hpp"
#include "cache.hpp"
#include "processor.hpp"

#define LIMIT 10

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "usage: ./msim <program_name>" << std::endl;
    exit(EXIT_FAILURE);
  }

  Instruction *program; int isize;
  std::tie(program, isize) = load_program(argv[1]);
  ICache icache(program, isize);
  DCache dcache(64);

  std::cout << icache;

  Processor cpu1(icache, dcache); Processor *cpu   = &cpu1;
  Processor cpu2(icache, dcache); Processor *n_cpu = &cpu2;

  for (int i = 0; i < LIMIT; ++i)
  {
    std::cout << "(t = " << i << ") " << (*cpu) << std::endl;
    cpu->tick(*n_cpu);
    *cpu = *n_cpu;
  }
  std::cout << "(t = " << LIMIT << ") " << (*cpu) << std::endl;

  return 0;
}
