#ifndef PARSER_H
#define PARSER_H

#include "instruction.hpp"

struct InstDef
{
  char opcode[16];
  int num_params;
  char params[3];
};

// ==================================
// === INSTRUCTION SET DEFINITION === 
// ==================================

// o : oooo oo-- ---- ---- ---- ---- ---- ----
// s : ---- --ss sss- ---- ---- ---- ---- ----
// t : ---- ---- ---t tttt ---- ---- ---- ----
// d : ---- ---- ---- ---- dddd d--- ---- ----
// i : ---- ---- ---- ---- iiii iiii iiii iiii

#define ISIZE 64
const InstDef instset[ISIZE] = {
  { "nop" , 0, { '\0'          }},
  { "add" , 3, { 'd', 's', 't' }},
  { "addi", 3, { 't', 's', 'i' }},
  { "sub" , 3, { 'd', 's', 't' }},
  { "subi", 3, { 't', 's', 'i' }},
  { "j"   , 1, { 'i'           }},
  { "jnez", 2, { 's', 'i'      }},
  { "ldi" , 3, { 't', 's', 'i' }},
  { "sti" , 3, { 't', 's', 'i' }},
  { "xor" , 3, { 'd', 's', 't' }},
};

// =========================
// === END OF DEFINITION ===
// =========================

class Parser
{
  public:
    struct Label
    {
      int p;
      char text[256];
    };

  public:
    Instruction * load_program(char *filename);

  private:
    int lnum;
    int inum;
    Label *ltable;

    void get_metadata(FILE *code);
    void get_labels(FILE *code);
    void get_program(FILE *code, Instruction *program);
    
    int regval(char *arg);
    int numval(char *arg);
    int labelval(char *label);

    Instruction parse_inst(const InstDef *inst, int line);
};

#endif
