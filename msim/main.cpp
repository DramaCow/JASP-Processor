#include <iostream>
#include <cstdio>

#include "programloader.hpp"
#include "processor.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "usage: ./start <program_name>.s" << std::endl;
    exit(EXIT_FAILURE);
  }

  char *program_name = argv[1]; 
  uint8_t *program;
  int size = load_program(program_name, &program);

  Memory imem(128); imem.copy(0, program, size);
  Memory dmem(128);
  //std::cout << imem << std::endl << std:: endl << dmem << std::endl;

  Processor cpu(imem, dmem);
  const int limit = 20;
  for (int i = 0; i < limit; ++i)
  {
    std::cout << "t = " << i << ':' << std::endl << cpu << std::endl;
    cpu.tick();
  }
  std::cout << "t = " << limit << ':' << std::endl << cpu << std::endl;

  return 0;
}
