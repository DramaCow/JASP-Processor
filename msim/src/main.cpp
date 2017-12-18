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

  Processor cpu(icache, dcache); 
  Processor n_cpu = cpu; 

  int t = 0;
  bool done = false;

#ifdef DEBUG
  for (; t < LIMIT && !done; ++t)
  {
    std::cout << "(t" << t << ") " << cpu << std::endl;
    done = cpu.tick(n_cpu);
    cpu = n_cpu;
  }
  std::cout << "(t" << t << ") " << cpu << std::endl;
#else
  std::cout << "(t" << t << ") " << cpu.rrf << std::endl;
  for (; t < LIMIT && !done; ++t)
  {
    done = cpu.tick(n_cpu);
    if (!(cpu.rrf == n_cpu.rrf))
    {
      std::cout << "(t" << t << ") " << n_cpu.rrf << std::endl;
    }
    cpu = n_cpu;
  }
  std::cout << "(t" << t << ") " << cpu.rrf << std::endl;
#endif

#ifdef EXE_TRACE
  for (std::size_t i = 0; i < cpu.exe.size(); ++i)
  {
    std::cout << cpu.exe[i] << std::endl;
  }
#endif

  return 0;
}
