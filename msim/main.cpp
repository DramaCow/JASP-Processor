#include <iostream>

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

  for (int i = 0; i < size; i += 4)
  {
    std::cout << program[i] << std::endl;
  }

  Memory mem(512);
  Processor cpu(mem);

  return 0;
}
