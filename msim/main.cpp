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
  int *program;
  int num_inst = load_program(program_name, &program);

  Memory mem;
  Processor cpu(mem);

  return 0;
}
