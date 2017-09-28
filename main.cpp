#include <iostream>
#include <string>

int load_program(std::string program_name);

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "usage: ./start <program_name>.s" << std::endl;
    exit(EXIT_FAILURE);
  }

  std::string program_name(argv[1]); 
  load_program(program_name);

  return 0;
}

int load_program(std::string program_name)
{
  FILE* f = fopen(program_name.c_str(), "r");

  if (f == nullptr)
  {
    std::cout << "Error while opening the file." << std::endl;
    exit(EXIT_FAILURE);
  }

  fclose(f);

  return 0;
}
