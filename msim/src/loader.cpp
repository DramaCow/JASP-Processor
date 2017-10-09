#include "loader.hpp"
#include <iostream>

int load(const char *filename, uint8_t **data)
{
  FILE* f = fopen(filename, "rb");
  if (f == nullptr)
  {
    return -1;
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

void save(const char *filename, uint8_t **data, int size)
{
  FILE *f = fopen(filename, "wb");
  fwrite((*data), sizeof(uint8_t), size, f);
  fclose(f);
}
