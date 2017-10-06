#include "processor.hpp"
#include "isa.hpp"
#include <iomanip>

Processor::Processor(Memory &imem, Memory &dmem) :
  state(0),
  imem(imem),
  dmem(dmem),
  pc(0), npc(0),

  oreg(0),
  a_latch(0),
  b_latch(0),
  otype(OP_ADD),
  waddr(0),
  we(false),
  abs_branch(false),
  var_pc_inc(false),
  t_latch(0),
  c_latch(0),

  cycles(0),
  instructions_executed(0)
{
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << "{\n"
     << "  pc = " << std::dec << cpu.pc << '\n'
     << "  oreg = " << std::setfill('0') << std::setw(8) << std::hex << cpu.oreg << '\n'
//     << "  a_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.a_latch << '\n'
//     << "  b_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.b_latch << '\n'
//     << "  t_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.t_latch << '\n'
//     << "  waddr = " << std::dec << cpu.waddr << '\n'
//     << "  we = " << cpu.we << '\n'
     << "  regfile = \n    " << cpu.regfile << '\n'
     << "}\n";
/*
  os << "=== statistics ===\n";
  os << "cycles = " << cpu.cycles << '\n';
  os << "instructions_executed = " << cpu.instructions_executed << '\n';
  os << "instructions_per_cycle = " << ((double)cpu.instructions_executed / (double)cpu.cycles);
*/
  switch (cpu.state)
  {
    case 0: os << "fetching..."; break;
    case 1: os << "decoding..."; break;
    case 2: os << "executing..."; break;
    case 3: os << "writing..."; break;
  }
  os << std::dec;
  return os;
}

void Processor::tick()
{
  switch (state)
  {
    case 0/*FETCH*/:     fetch();     break;
    case 1/*DECODE*/:    decode();    break;
    case 2/*EXECUTE*/:   execute();   break;
    case 3/*WRITEBACK*/: writeback(); break;
    default: {
      std::cerr << "*** entered invalid pipeline state ***\n";
      exit(EXIT_FAILURE);
    }
  }
  state = (state + 1) % 4;
  cycles++;
}

void Processor::fetch()
{
  oreg = imem[pc];
}

void Processor::decode()
{
  uint32_t opcode = (oreg >> (32 - 6)) & 0x3f; // first 6 bits
  uint32_t s = (oreg >> (32 - 11)) & 0x1f;
  uint32_t t = (oreg >> (32 - 16)) & 0x1f;
  uint32_t d = (oreg >> (32 - 21)) & 0x1f;
  uint32_t i = oreg & 0xffff;
  uint32_t a = oreg & 0x3fffff;

  switch (opcode)
  {
    case NOP: {
      break;
    }

    case ADD: {
      std::tie(a_latch, b_latch) = regfile.foo(s, t, 0, 0, false);
      otype = OP_ADD;
      waddr = d;
      we = true;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    case ADDI: {
      std::tie(a_latch, std::ignore) = regfile.foo(s, 0, 0, 0, false);
      b_latch = i;
      otype = OP_ADD;
      waddr = t;
      we = true;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    case SUB: {
      std::tie(a_latch, b_latch) = regfile.foo(s, t, 0, 0, false);
      otype = OP_SUB;
      waddr = d;
      we = true;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    case SUBI: {
      std::tie(a_latch, std::ignore) = regfile.foo(s, 0, 0, 0, false);
      b_latch = i;
      otype = OP_SUB;
      waddr = t;
      we = true;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    case CMP: {
      std::tie(a_latch, b_latch) = regfile.foo(s, t, 0, 0, false);
      otype = OP_CMP;
      we = false;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    case CMPI: {
      std::tie(a_latch, std::ignore) = regfile.foo(s, t, 0, 0, false);
      b_latch = i;
      otype = OP_CMP;
      we = false;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    case J: {
      a_latch = a << 2; // same as *4, makes address word aligned
      b_latch = 0;
      otype = OP_ADD;
      we = false;
      abs_branch = true;
      var_pc_inc = false;
      break;
    }

    case JNE: {
      a_latch = a << 2; // same as *4, makes address word aligned
      b_latch = 0;
      otype = OP_ADD;
      we = false;
      abs_branch = (c_latch & 0x1) == 0;
      var_pc_inc = false;
      break;
    }

    case LD: {
      break;
    }

    case SR: {
      break;
    }

    case XOR: {
      std::tie(a_latch, b_latch) = regfile.foo(s, t, 0, 0, false);
      otype = OP_XOR;
      waddr = d;
      we = true;
      abs_branch = false;
      var_pc_inc = false;
      break;
    }

    default: {
      std::cerr << "*** invalid opcode ***\n";
      exit(EXIT_FAILURE);
    }
  }
}

void Processor::execute()
{
  switch (otype)
  {
    case OP_ADD: t_latch = a_latch + b_latch; break;
    case OP_SUB: t_latch = a_latch - b_latch; break;
    case OP_XOR: t_latch = a_latch ^ b_latch; break;
    case OP_CMP: {
      c_latch = 0x3 & (
        (a_latch >  b_latch) << 2 & 
        (a_latch <  b_latch) << 1 & 
        (a_latch == b_latch) << 0
      );
      break;
    }
    default: {
      std::cerr << "*** invalid operation ***\n";
      exit(EXIT_FAILURE);
    }
  }

  if (abs_branch)
  {
    npc = t_latch;
  }
  else if (var_pc_inc)
  {
    npc += t_latch;
  }
  else
  {
    npc += 4;
  }

  instructions_executed++;
}

void Processor::writeback()
{
  regfile.foo(0, 0, waddr, t_latch, we);
  we = false;
  pc = npc;
}
