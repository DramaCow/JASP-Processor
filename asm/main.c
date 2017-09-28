#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "label.h"
#include "instruction.h"

int regval(char *arg);
int addrval(char *arg);
int labelval(char *label, LabelTable *table);

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("usage: ./asm <code_file>.x\n");
    exit(EXIT_FAILURE);
  }

  FILE *code = fopen(argv[1], "r");
  
  if (code == NULL) 
  {
    printf("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  LabelTable ltable = { NULL, NULL };
  Program program   = { NULL, NULL };

  char buffer[256];
  char *tok = NULL;

  // populate label table
  for (int ln = 1, p = 0; fgets(buffer, sizeof(buffer), code) != NULL; ++ln)
  {
    tok = strtok(buffer, " \t\n\0");

    // skip blank lines or comments
    if (tok == NULL || tok[0] == ';') 
    {
      continue;
    }

    // labels start with colons
    if (tok[0] == ':') 
    {
      add_label(&ltable, p, tok);

      // nothing else must exist on the line - else error
      tok = strtok(NULL, " \t\n\0");
      if (tok != NULL) {
        printf(" *** Error whilst parsing labels at line: %d ***\n", ln);
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      p++;
    }
  }

  print_labels(&ltable);

  rewind(code);

  // populate instructions
  for (int ln = 1, p = 0; fgets(buffer, sizeof(buffer), code) != NULL; ++ln)
  {
    tok = strtok(buffer, " \t\n\0");

    // skip blank lines or comments
    if (tok == NULL || tok[0] == ';') 
    {
      continue;
    }

    // labels start with colons
    if (tok[0] == ':') 
    {
      add_label(&ltable, p, tok);

      // nothing else must exist on the line - else error
      tok = strtok(NULL, " \t\n\0");
      if (tok != NULL) {
        printf(" *** Error whilst parsing labels at line: %d ***\n", ln);
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      p++;
    }
  }

  fclose(code);

  return 0;
}

int regval(char *arg) {
  if (arg[0] == 'r') {
    for (size_t i = 1; i < strlen(arg); i++) {
      if (arg[i] < '0' || arg[i] > '9') {
        return -1;
      }
    }

    int reg = 0;
    // TODO: it's dangerous to add 1 (needed to skip 'r')
    sscanf(arg + sizeof(char) * 1, "%d", &reg);
    return reg;
  }

  return -1;
}

int addrval(char *arg) {
  for (size_t i = 0; i < strlen(arg); i++) 
  {
    if (arg[i] < '0' || arg[i] > '9') 
    {
      return -1;
    }
  }

  int addr = 0;
  sscanf(arg, "%d", &addr);
  return addr;
}

int labelval(char *label, LabelTable *table) {
  int p = -1;

  for (Label *e = table->head; e != NULL; e = e->next) 
  {
    if (strcmp(label, e->label) == 0) 
    {  
      p = e->line;
      break;
    }
  }
 
  return p;
}
