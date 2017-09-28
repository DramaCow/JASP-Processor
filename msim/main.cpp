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

  return 0;
}

int load_program(char *program_name, int **program)
{
  FILE* f = fopen(program_name, "wr");

  if (f == nullptr)
  {
    std::cout << "Error while opening the file." << std::endl;
    exit(EXIT_FAILURE);
  }

  int num_inst = 0;
  fread(&num_inst, sizeof(int), 1, f);
  (*program) = new int[num_inst];

  for (int i = 0; i < num_inst; ++i)
  {
    (*program)[i] = 0;
  }

  fclose(f);

  return num_inst;
}
