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

  { "mov" , 2, { 'd', 's'      }},
  { "movi", 2, { 'd', 'i'      }},

  { "add" , 3, { 'd', 's', 't' }},
  { "addi", 3, { 'd', 's', 'i' }},
  { "sub" , 3, { 'd', 's', 't' }},
  { "subi", 3, { 'd', 's', 'i' }},
  { "mul" , 3, { 'd', 's', 't' }},
  { "muli", 3, { 'd', 's', 'i' }},

  { "xor" , 3, { 'd', 's', 't' }},

  { "b"   , 1, { 'i'           }},
  { "beq" , 3, { 's', 't', 'i' }},
  { "bneq", 3, { 's', 't', 'i' }},
  { "blt" , 3, { 's', 't', 'i' }},
  { "ble" , 3, { 's', 't', 'i' }},
  { "bgt" , 3, { 's', 't', 'i' }},
  { "bge" , 3, { 's', 't', 'i' }},

  { "lw"  , 3, { 'd', 's', 'i' }},
  { "sw"  , 3, { 't', 's', 'i' }},

  { "end" , 0, { '\0'          }},
};

// =========================
// === END OF DEFINITION ===
// =========================

std::tuple<Instruction*,int> load_program(char *filename);
std::tuple<int*,int> load_data(char *filename);

#endif
