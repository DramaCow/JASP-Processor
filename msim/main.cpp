#include <iostream>
#include <string>

int load_program(char *program_name, int **program);

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
  std::cout << num_inst << std::endl;

  return 0;
}

int load_program(char *program_name, int **program)
{
  FILE* f = fopen(program_name, "rb");
  if (f == nullptr)
  {
    std::cout << "Error while opening the file." << std::endl;
    exit(EXIT_FAILURE);
  }

  // count number of bytes in file
  fseek(f, 0L, SEEK_END);
  int size = ftell(f);
  int num_inst = size / 4;
  rewind(f);

  // read program
  (*program) = new int[num_inst];
  for (int i = 0; i < num_inst; ++i)
  {
    (*program)[i] = 0;
  }

  fclose(f);

  return num_inst;
}
