#include <registers.h>

#define INIT_REG_VAL 0

Register::Registers() :
  pc(INIT_REG_VAL), npc(INIT_REG_VAL),
  sp(INIT_REG_VAL),
  fp(INIT_REG_VAL),
  lr(INIT_REG_VAL)
{
}
