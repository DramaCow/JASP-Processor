#include <iostream>
#include <cstdio>

//#include "loader.hpp"
#include "parser.hpp"
#include "cache.hpp"
#include "processor.hpp"

#define LIMIT 100000

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "usage: ./msim <program_name> [<data_name>]?" << std::endl;
    exit(EXIT_FAILURE);
  }

  Instruction *program; int isize;
  std::tie(program, isize) = load_program(argv[1]);
  ICache icache(program, isize);

  DCache dcache(64);
  if (argc >= 3)
  {
    int *data; int dsize;
    std::tie(data, dsize) = load_data(argv[2]);
    dcache.initialise(data, dsize);
  }

  Processor cpu1(icache, dcache); Processor *cpu   = &cpu1;
  Processor cpu2(icache, dcache); Processor *n_cpu = &cpu2;

  int t = 0;
  bool done = false;

#ifndef DEBUG
  std::cout << "(t" << t << ") " << (*cpu) << std::endl;
#endif

  for (; t < LIMIT && !done; ++t)
  {
#ifdef DEBUG
    std::cout << "(t" << t << ") " << (*cpu) << std::endl;
#endif
    done = cpu->tick(*n_cpu);
#ifndef DEBUG
    if (!(cpu->rrf == n_cpu->rrf))
    {
      std::cout << "(t" << t << ") " << (*n_cpu) << std::endl;
    }
#endif
    *cpu = *n_cpu;
  }
  std::cout << "(t" << t << ") " << (*cpu) << std::endl;

  return 0;
}
