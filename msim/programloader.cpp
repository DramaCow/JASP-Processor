#include "programloader.hpp"
#include <iostream>

int load_program(char *program_name, uint8_t **program)
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
  rewind(f);

  // read program
  (*program) = new uint8_t[size];
  fread((*program), sizeof(uint8_t), size, f);

//  for (int i = 0; i < size; ++i)
//  {
//    printf("%02x", (*program)[i]);
//    if (i % 2 == 1)
//    {
//      printf(" ");
//    }
//  }

  fclose(f);

  return size;
}
