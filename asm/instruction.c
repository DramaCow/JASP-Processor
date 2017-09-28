#include "instruction.h"
#include <stdlib.h>
#include <stdio.h>

void add_inst(Program *program, int p, int opcode) {
  if (program->tail == NULL) {
    program->head = (Inst*)malloc(sizeof(Inst));
    program->head->p = p;
    program->head->opcode = opcode; 
    program->tail = program->head;

    return;
  }
  
  Inst *tmp = program->tail; 
  program->tail = (Inst*)malloc(sizeof(Inst));
  program->tail->p = p;
  program->tail->opcode = opcode;
  tmp->next = program->tail;
}  

void free_program(Inst *inst) {
  if (inst != NULL) {
    free_program(inst->next);
  }
  free(inst); 
}

void print_program(Program *program) {
  for (Inst *e = program->head; e != NULL; e = e->next) {
    printf("%d: %d\n", e->p, e->opcode);
  }
}
