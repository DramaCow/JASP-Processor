#include <iostream>
#include <cstdio>
#include <fstream>

#include "parser.hpp"
#include "cache.hpp"
#include "processor.hpp"

#define LIMIT 100000

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    std::cerr << "usage: ./msim <program_name> <data_name>" << std::endl;
    exit(EXIT_FAILURE);
  }

  Instruction *program; int isize;
  std::tie(program, isize) = load_program(argv[1]);
  ICache icache(program, isize);

  int *data; int dsize;
  std::tie(data, dsize) = load_data(argv[2]);
  DCache dcache(data, dsize);
  SAC l1cache(L1_NUM_SETS, L1_SET_SIZE);
  SAC l2cache(L2_NUM_SETS, L2_SET_SIZE);
  MEM mem(data, dsize);
  delete data;

  Processor cpu(icache, dcache, l1cache, l2cache, mem); 
  Processor n_cpu(icache, dcache, l1cache, l2cache, mem); 

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

  std::cout << l1cache << std::endl;
  std::cout << l2cache << std::endl;

  cpu.mu.complete_writethrough();
  std::ofstream file("mem.txt");
  file << cpu.mem;
  file.close();

  return 0;
}
