#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void get_metadata(FILE *file, int *size);
void get_data(FILE *file, int size, uint32_t *data);
int numval(char *arg);

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("usage: ./data <data_file>.d\n");
    exit(EXIT_FAILURE);
  }

  FILE *file = fopen(argv[1], "r");
  
  if (file == NULL) 
  {
    printf("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  int size;
  get_metadata(file, &size);
  rewind(file);

  uint32_t *data = (uint32_t*)malloc(sizeof(uint32_t)*size);

  get_data(file, size, data);

  printf("DATA\n");
  for (int i = 0; i < size; ++i)
  {
    printf(" %d %08x\n", i, data[i]);
  }

  fclose(file);

  FILE *out = fopen("d.hex", "wb");
  for (int i = 0; i < size; i++)
  {
    uint8_t b0 = (data[i] >>  0) & 0xff; 
    uint8_t b1 = (data[i] >>  8) & 0xff; 
    uint8_t b2 = (data[i] >> 16) & 0xff; 
    uint8_t b3 = (data[i] >> 24) & 0xff; 

    //printf("%02x%02x%02x%02x ", b0, b1, b2, b3);

    // little endian
    fwrite(&b0, sizeof(uint8_t), 1, out);
    fwrite(&b1, sizeof(uint8_t), 1, out);
    fwrite(&b2, sizeof(uint8_t), 1, out);
    fwrite(&b3, sizeof(uint8_t), 1, out);
  }
  fclose(out);

  return 0;
}

void get_metadata(FILE *file, int *size)
{
  char buffer[256];
  char *tok = NULL;

  (*size) = 0;

  while (fgets(buffer, sizeof(buffer), file) != NULL)
  {
    tok = strtok(buffer, " \t\n\0");
    if (tok != NULL) 
    {
      (*size)++;
    }
  }
}

void get_data(FILE *file, int size, uint32_t *data)
{
  char buffer[256];
  char *tok = NULL;

  int p = 0;

  while (fgets(buffer, sizeof(buffer), file) != NULL)
  {
    tok = strtok(buffer, " \t\n\0");
    if (tok != NULL) 
    {
      data[p++] = numval(tok);
    }
  }
}

int numval(char *arg) {
  for (size_t i = 0; i < strlen(arg); i++) 
  {
    if (arg[i] < '0' || arg[i] > '9') 
    {
      printf("*** error - not a valid number ***\n");
      exit(EXIT_FAILURE);
    }
  }

  int num = 0;
  sscanf(arg, "%d", &num);
  return num;
}
