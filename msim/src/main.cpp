#include <iostream>
#include <cstdio>

#include "loader.hpp"
#include "processor.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "usage: ./start <program_name>.s" << std::endl;
    exit(EXIT_FAILURE);
  }

  char *program_name = argv[1]; 
  uint8_t *program;
  int isize = load(program_name, &program);

  Memory imem(256); imem.copy(0, program, isize);
  Memory dmem(256);

  if (argc < 3)
  {
    std::cout << "no data argument given - using default dmem" << std::endl;
  }
  else
  {
    char *data_name = argv[2];
    uint8_t *data;
    int dsize = load(data_name, &data);
    dmem.copy(0, data, dsize);
  }

  Processor cpu1(imem, dmem); Processor *cpu   = &cpu1;
  Processor cpu2(imem, dmem); Processor *n_cpu = &cpu2;

  const int limit = 100;
  for (int i = 0; i < limit; ++i)
  {
//    if (i % 5 == 0)
//      std::cout << "(t = " << i << ") " << (*cpu) << std::endl;
    cpu->tick(*n_cpu);
    std::swap(cpu, n_cpu);
    n_cpu->regfile = cpu->regfile; // next state needs copy of up-to-date register file
  }
  std::cout << "(t = " << limit << ") " << (*cpu) << std::endl;

  std::cout << imem << std::endl;
  std::cout << dmem << std::endl;

  return 0;
}
