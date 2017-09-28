#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

typedef struct Inst {
  int line;          // line the instruction is found on
  uint8_t opcode;    // 3 bit operation
  struct Inst *next; // pointer to next instruction
} Inst;

typedef struct Program {
  Inst *head;
  Inst *tail;
} Program;

void add_inst(Program *program, int line, int opcode);
void free_program(Inst *inst);
void print_program(Program *program);

#endif
