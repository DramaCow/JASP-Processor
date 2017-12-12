#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include "parser.hpp"

// ================================
// === HELPER CLASS DECLARATION ===
// ================================

class Parser
{
  public:
    struct Label
    {
      int p;
      char text[256];
    };

  public:
    std::tuple<Instruction*,int> load_program(char *filename);

  private:
    int lnum;
    int inum;
    Label *ltable;

    void get_metadata(FILE *code);
    void get_labels(FILE *code);
    void get_program(FILE *code, Instruction *program);
    
    int regval(char *arg);
    int numval(char *arg, int *val);
    int labelval(char *label);

    Instruction parse_inst(const InstDef *inst, int line);
};

// ==============================
// === FUNCTION WE CARE ABOUT ===
// ==============================

std::tuple<Instruction*,int> load_program(char *filename)
{
  Parser parser;
  return parser.load_program(filename);
}

std::tuple<int*,int> load_data(char *filename)
{
  FILE *datafile = fopen(filename, "r");
  if (datafile == NULL) 
  {
    printf("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  int size = 0;
  char buffer[256];

  while (fgets(buffer, sizeof(buffer), datafile) != NULL)
  {
    size++;
  }

  rewind(datafile);

  int *data = new int[size];
  for (int i = 0; fgets(buffer, sizeof(buffer), datafile) != NULL; ++i)
  {
    sscanf(buffer, "%d", &data[i]);
  }

  fclose(datafile);

  return std::make_tuple(data, size);
}

// ================================
// === HELPER CLASS DEFINITIONS ===
// ================================

std::tuple<Instruction*,int> Parser::load_program(char *filename)
{
  FILE *code = fopen(filename, "r");
  if (code == NULL) 
  {
    printf("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  get_metadata(code);
  rewind(code);

  this->ltable = new Label[this->lnum];
  get_labels(code);
  rewind(code);

  Instruction *program = new Instruction[this->inum];
  get_program(code, program);

  fclose(code);
  delete[] this->ltable;

  return std::make_tuple(program, this->inum);
}

void Parser::get_metadata(FILE *code)
{
  char buffer[256];
  char *tok = NULL;

  this->lnum = 0;
  this->inum = 0;

  while (fgets(buffer, sizeof(buffer), code) != NULL)
  {
    tok = strtok(buffer, " \t\n\0");

    if (tok != NULL && tok[0] != ';')
    {
      if (tok[0] == ':')
      {
        this->lnum++;
      }
      else
      {
        this->inum++;
      }
    }
  }
}

void Parser::get_labels(FILE *code)
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
        // update table shelf
        this->ltable[i].p = addr;
        strncpy(this->ltable[i].text, tok, sizeof(tok)*sizeof(char));
        i++;

        // nothing else must exist on the line - else error
        tok = strtok(NULL, " \t\n\0");
        if (tok != NULL) 
        {
          printf(" *** Error whilst parsing labels at line: %d ***\n", ln);
          exit(EXIT_FAILURE);
        }
      }
      // otherwise, it's an Instruction
      else
      {
        //addr += 4;
        addr += 1;
      }
    }
  }
}

void Parser::get_program(FILE *code, Instruction *program)
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

      // verify Instruction is valid
      for (int i = 0; i < ISIZE; ++i)
      {
        if (strcmp(tok, instset[i].opcode) == 0)
        {
          id = i;
          break;
        }
      }

      // return if valid Instruction is provided
      if (id == -1)
      {
        printf("*** error on line(%d) - %s is not a valid operation. ***\n", ln, tok);
        exit(EXIT_FAILURE);
      }

      program[p++] = parse_inst(&instset[id], ln);
    }
  }
}

int Parser::regval(char *arg) {
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

int Parser::numval(char *arg, int *val) {
  int size = strlen(arg);
  for (int i = 0; i < size; i++) 
  {
    if (i == 0 && arg[0] == '-' && size > 1) {
      continue;
    }
    if (arg[i] < '0' || arg[i] > '9') 
    {
      return 0;
    }
  }

  int num = 0;
  sscanf(arg, "%d", &num);
  (*val) = num;

  return 1;
}

int Parser::labelval(char *label) {
  int p = -1;

  for (int i = 0; i < this->lnum; ++i) 
  {
    if (strcmp(label, this->ltable[i].text) == 0) 
    {  
      p = this->ltable[i].p;
      break;
    }
  }
 
  return p;
}

Instruction Parser::parse_inst(const InstDef *inst, int line)
{
  Instruction Instruction;
  Instruction.opcode = std::string(inst->opcode);

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

    if (inst->params[p] == 'd' || inst->params[p] == 's' || inst->params[p] == 't')
    {
      val = regval(tok);
      if (val == -1)
      {
        printf("*** error on line(%d) - invalid register vALUe. ***\n", line);
        exit(EXIT_FAILURE);
      }
    }
    else if (inst->params[p] == 'i' || inst->params[p] == 'e')
    {
      if (tok[0] == ':')
      {
        val = labelval(tok);
        if (val == -1)
        {
          printf("*** error on line(%d) - invalid address vALUe. ***\n", line);
          exit(EXIT_FAILURE);
        }
      }
      else
      {
        if (!numval(tok, &val))
        {
          printf("*** error on line(%d) - invalid address vALUe. ***\n", line);
          exit(EXIT_FAILURE);
        }
      }
    }
    else
    {
      printf("*** error on line(%d) - schematic Instruction param invalid. ***\n", line);
      exit(EXIT_FAILURE);
    }

    Instruction.params.push_back(val);
  }

  tok = strtok(NULL, " \t\n\0");

  if (tok != NULL)
  {
    printf("*** error on line(%d) - too many params. ***\n", line);
    exit(EXIT_FAILURE);
  }

  return Instruction;
}
