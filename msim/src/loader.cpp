#include "loader.hpp"
#include <iostream>

int load(char *filename, uint8_t **data)
{
  FILE* f = fopen(filename, "rb");
  if (f == nullptr)
  {
    std::cout << "Error while opening the file." << std::endl;
    exit(EXIT_FAILURE);
  }

  // count number of bytes in file
  fseek(f, 0L, SEEK_END);
  int size = ftell(f);
  rewind(f);

  // read data
  (*data) = new uint8_t[size];
  fread((*data), sizeof(uint8_t), size, f);

  /*
  for (int i = 0; i < size; ++i)
  {
    printf("%02x", (*data)[i]);
    if (i % 2 == 1)
    {
      printf(" ");
    }
  }
  */

  fclose(f);

  return size;
}
