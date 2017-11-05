#ifndef PARSER_H
#define PARSER_H

#include "instruction.hpp"
#include <tuple>

struct InstDef
{
  char opcode[16];
  int num_params;
  char params[3];
};

// ==================================
// === INSTRUCTION SET DEFINITION === 
// ==================================

/* KEY:
 *  d = destination register
 *  s = source register 1
 *  t = source register 2
 *  i = immediate operand
 */

#define ISIZE 64
const InstDef instset[ISIZE] = {
  { "nop" , 0, { '\0'          }},
  { "add" , 3, { 'd', 's', 't' }},
  { "addi", 3, { 'd', 's', 'i' }},
  { "sub" , 3, { 'd', 's', 't' }},
  { "subi", 3, { 'd', 's', 'i' }},
  { "xor" , 3, { 'd', 's', 't' }},
//  { "j"   , 1, {           'e' }},
//  { "jnez", 2, {      's', 'e' }},
//  { "jlth", 3, { 's'  's', 'e' }},
//  { "ldi" , 3, { 'd', 's', 'i' }},
//  { "sti" , 3, { 's', 't', 'e' }},
};

// =========================
// === END OF DEFINITION ===
// =========================

std::tuple<Instruction*,int> load_program(char *filename);

#endif
