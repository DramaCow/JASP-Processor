#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct Inst
{
  char name[16];
  uint32_t opcode;
  int num_params;
  char params[3];
} Inst;

// ==================================
// === INSTRUCTION SET DEFINITION === 
// ==================================

// o : oooo oo-- ---- ---- ---- ---- ---- ----
// s : ---- --ss sss- ---- ---- ---- ---- ----
// t : ---- ---- ---t tttt ---- ---- ---- ----
// d : ---- ---- ---- ---- dddd d--- ---- ----
// i : ---- ---- ---- ---- iiii iiii iiii iiii
// a : ---- --aa aaaa aaaa aaaa aaaa aaaa aaaa

#define ISIZE 8
const Inst instset[ISIZE] = {
  { "add" , 0x00, 3, { 'd', 's', 't' }},
  { "addi", 0x01, 3, { 't', 's', 'i' }},
};

// =========================
// === END OF DEFINITION ===
// =========================

typedef struct Label
{
  int p;
  char text[256];
} Label;

void get_metadata(FILE *code, int *num_labels, int *num_inst);
void get_labels(FILE *code, Label *ltable);
void get_program(FILE *code, int num_labels, Label *ltable, uint32_t *program);

int regval(char *arg);
int numval(char *arg);
int labelval(char *label, int num_labels, Label *ltable);

uint32_t parse_inst(FILE *code, const Inst *inst, int depth, int line, int num_labels, Label *ltable);

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

  int num_labels;
  int num_inst;

  get_metadata(code, &num_labels, &num_inst);
  rewind(code);

  //printf("num_labels = %d\n", num_labels);
  //printf("num_inst = %d\n", num_inst);

  Label *ltable = (Label*)malloc(sizeof(Label)*num_labels);
  uint32_t *program = (uint32_t*)malloc(sizeof(uint32_t)*num_inst);

  get_labels(code, ltable);
  rewind(code);

  printf("LABELS\n");
  for (int i = 0; i < num_labels; ++i)
  {
    printf(" %d %s\n", ltable[i].p, ltable[i].text);
  }

  get_program(code, num_labels, ltable, program);

  printf("PROGRAM\n");
  for (int i = 0; i < num_inst; ++i)
  {
    printf(" %d %08x\n", i, program[i]);
  }

  fclose(code);

  FILE *out = fopen("a.hex", "wb");
  for (int i = 0; i < num_inst; i++)
  {
    uint8_t b0 = (program[i] >>  0) & 0xff; 
    uint8_t b1 = (program[i] >>  8) & 0xff; 
    uint8_t b2 = (program[i] >> 16) & 0xff; 
    uint8_t b3 = (program[i] >> 24) & 0xff; 

    //printf("%02x%02x%02x%02x ", b0, b1, b2, b3);

    // little endian
    fwrite(&b0, sizeof(uint8_t), 1, out);
    fwrite(&b1, sizeof(uint8_t), 1, out);
    fwrite(&b2, sizeof(uint8_t), 1, out);
    fwrite(&b3, sizeof(uint8_t), 1, out);
  }
  //fwrite(program, sizeof(uint32_t), num_inst, out);
  fclose(out);

  return 0;
}

void get_metadata(FILE *code, int *num_labels, int *num_inst)
{
  char buffer[256];
  char *tok = NULL;

  (*num_labels) = 0;
  (*num_inst)   = 0;

  while (fgets(buffer, sizeof(buffer), code) != NULL)
  {
    tok = strtok(buffer, " \t\n\0");

    if (tok != NULL && tok[0] != ';')
    {
      if (tok[0] == ':')
      {
        (*num_labels)++;
      }
      else
      {
        (*num_inst)++;
      }
    }
  }

  rewind(code);
}

void get_labels(FILE *code, Label *ltable)
{
  char buffer[256];
  char *tok = NULL;

  for (int ln = 1, addr = 0, i = 0; fgets(buffer, sizeof(buffer), code) != NULL; ++ln)
  {
    tok = strtok(buffer, " \t\n\0");

    if (tok != NULL && tok[0] != ';') 
    {
      // labels start with colons
      if (tok[0] == ':') 
      {
        // update table entry
        ltable[i].p = addr;
        strncpy(ltable[i].text, tok, sizeof(tok)*sizeof(char));
        i++;

        // nothing else must exist on the line - else error
        tok = strtok(NULL, " \t\n\0");
        if (tok != NULL) 
        {
          printf(" *** Error whilst parsing labels at line: %d ***\n", ln);
          exit(EXIT_FAILURE);
        }
      }
      // otherwise, it's an instruction
      else
      {
        addr += 4;
      }
    }
  }
}

void get_program(FILE *code, int num_labels, Label *ltable, uint32_t *program)
{
  char buffer[256];
  char *tok = NULL;

  for (int ln = 1, p = 0; fgets(buffer, sizeof(buffer), code) != NULL; ++ln)
  {
    tok = strtok(buffer, " \t\n\0");

    // skip blank lines, comments, or labels
    if (tok != NULL && tok[0] != ';' && tok[0] != ':') 
    {
      int id = -1;

      for (int i = 0; i < ISIZE; ++i)
      {
        if (strcmp(tok, instset[i].name) == 0)
        {
          id = i;
          break;
        }
      }

      if (id == -1)
      {
        printf("*** error on line(%d) - %s is not a valid operation. ***\n", ln, tok);
        exit(EXIT_FAILURE);
      }

      program[p++] = parse_inst(code, &instset[id], 0, ln, num_labels, ltable);
    }
  }
}

int regval(char *arg) {
  if (arg[0] == 'r') {
    for (size_t i = 1; i < strlen(arg); i++) {
      if (arg[i] < '0' || arg[i] > '9') {
        return -1;
      }
    }

    int reg = 0;
    sscanf(arg + sizeof(char), "%d", &reg);
    return reg;
  }

  return -1;
}

int numval(char *arg) {
  for (size_t i = 0; i < strlen(arg); i++) 
  {
    if (arg[i] < '0' || arg[i] > '9') 
    {
      return -1;
    }
  }

  int num = 0;
  sscanf(arg, "%d", &num);
  return num;
}

int labelval(char *label, int num_labels, Label *ltable) {
  int p = -1;

  for (int i = 0; i < num_labels; ++i) 
  {
    if (strcmp(label, ltable[i].text) == 0) 
    {  
      p = ltable[i].p;
      break;
    }
  }
 
  return p;
}

uint32_t parse_inst(FILE *code, const Inst *inst, int depth, int line, int num_labels, Label *ltable)
{
  uint32_t opcode = (0x3f & inst->opcode) << 26;

  char *tok;        

  for (int p = 0; p < inst->num_params; ++p)
  {
    int val;
    tok = strtok(NULL, " \t\n\0");

    if (tok == NULL)
    {
      printf("*** error on line(%d) - not enought params given: expecting %d, received %d. ***\n", line, inst->num_params, p-1);
      exit(EXIT_FAILURE);
    }

    switch (inst->params[p])
    {
      case 's': {
        val = regval(tok);
        if (val == -1)
        {
          printf("*** error on line(%d) - invalid register value. ***\n", line);
          exit(EXIT_FAILURE);
        }

        opcode |= (0x1f & val) << 21;

        break;
      }

      case 't': {
        val = regval(tok);
        if (val == -1)
        {
          printf("*** error on line(%d) - invalid register value. ***\n", line);
          exit(EXIT_FAILURE);
        }

        opcode |= (0x1f & val) << 16;

        break;
      }

      case 'd': {
        val = regval(tok);
        if (val == -1)
        {
          printf("*** error on line(%d) - invalid register value. ***\n", line);
          exit(EXIT_FAILURE);
        }

        opcode |= (0x1f & val) << 11;

        break;
      }

      case 'i': {
        if (tok[0] == ':')
        {
          val = labelval(tok, num_labels, ltable);
        }
        else
        {
          val = numval(tok);
        }
        if (val == -1)
        {
          printf("*** error on line(%d) - invalid address value. ***\n", line);
          exit(EXIT_FAILURE);
        }

        opcode |= (0xffff & val);

        break;
      }

      case 'a': {
        if (tok[0] == ':')
        {
          val = labelval(tok, num_labels, ltable);
        }
        else
        {
          val = numval(tok);
        }
        if (val == -1)
        {
          printf("*** error on line(%d) - invalid address value. ***\n", line);
          exit(EXIT_FAILURE);
        }

        opcode |= (0x3ffffff & val);

        break;
      }

      default: {
        printf("*** error on line(%d) - schematic instruction param invalid. ***\n", line);
        exit(EXIT_FAILURE);
      }
    }
  }

  tok = strtok(NULL, " \t\n\0");

  if (tok != NULL)
  {
    printf("*** error on line(%d) - too many params. ***\n", line);
    exit(EXIT_FAILURE);
  }

  return opcode;
}
