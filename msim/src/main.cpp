#include <iostream>
#include <cstdio>

#include "loader.hpp"
#include "processor.hpp"

#define LIMIT 20

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cerr << "usage: ./msim <program_name>" << std::endl;
    exit(EXIT_FAILURE);
  }

  Memory imem(256); 
  {
    const char *program_name = argv[1]; 
    uint8_t *program;
    int isize = load(program_name, &program);
    if (isize < 0)
    {
      std::cerr << "*** Error! " << program_name << " is not a valid binary input. ***" << std::endl;
      exit(EXIT_FAILURE);
    }
    imem.copy(0, program, isize);
  }

  Memory dmem(256);
  {
    uint8_t *data;
    int dsize = load("d.hex", &data);
    if (dsize < 0)
    {
      std::cout << "> Data file not found, initialising with zeroes." << std::endl;
    }
    else
    {
      dmem.copy(0, data, dsize);
    }
  }

  Processor cpu1(imem, dmem); Processor *cpu   = &cpu1;
  Processor cpu2(imem, dmem); Processor *n_cpu = &cpu2;

  for (int i = 0; i < LIMIT; ++i)
  {
//    if (i % 5 == 0)
      std::cout << "(t = " << i << ") " << (*cpu) << std::endl;

    cpu->tick(*n_cpu);
    std::swap(cpu, n_cpu);
    n_cpu->regfile = cpu->regfile; // next state needs copy of up-to-date register file
  }
  std::cout << "(t = " << LIMIT << ") " << (*cpu) << std::endl;

  std::cout << "instruction cache = " << std::endl;
  std::cout << imem << std::endl;
  std::cout << "data cache = " << std::endl;
  std::cout << dmem << std::endl;

  save("d.hex", &dmem.bytes, dmem.capacity);

  return 0;
}