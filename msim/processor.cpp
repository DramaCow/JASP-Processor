#include "processor.hpp"
#include "isa.hpp"
#include <iomanip>

Processor::Processor(Memory &imem, Memory &dmem) :
  state(FETCH),
  imem(imem),
  dmem(dmem),
  pc(0), npc(0),
  oreg(0),

  a_latch(0),
  b_latch(0),
  t_latch(0),

  cycles(0),
  instructions_executed(0)
{
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << "state = " << cpu.state << '\n';
  os << "pc = " << std::dec << cpu.pc << '\n';
  os << "oreg = " << std::setfill('0') << std::setw(8) << std::hex << cpu.oreg << '\n';
  os << cpu.regfile << '\n';
  os << "=== statistics ===\n";
  os << "cycles = " << cpu.cycles << '\n';
  os << "instructions_executed = " << cpu.instructions_executed << '\n';
  os << "instructions_per_cycle = " << ((double)cpu.instructions_executed / (double)cpu.cycles) << '\n';
  return os;
}

void Processor::tick()
{
  switch (state)
  {
    case FETCH: {
      fetch(); 
      state = DECODE;
      break;
    }

    case DECODE: {
      decode();
      state = EXECUTE;
      break;
    }

    case EXECUTE: {
      execute(); 
      state = WRITEBACK;

      instructions_executed++;
      break;
    }

    case WRITEBACK: {
      writeback(); 
      state = FETCH;
      break;
    }

    default: {
      std::cerr << "*** entered invalid pipeline state ***\n";
      exit(EXIT_FAILURE);
    }
  }

  cycles++;
}

void Processor::fetch()
{
  oreg = imem[pc];
  npc += 4;
}

void Processor::decode()
{
  uint32_t opcode = (oreg >> (32 - 6)) & 0x3f; // first 6 bits
  uint32_t s = (oreg >> (32 - 11)) & 0x1f;
  uint32_t t = (oreg >> (32 - 16)) & 0x1f;
  uint32_t i = oreg & 0xffff;
  uint32_t a = oreg & 0x3fffff;

  switch (opcode)
  {
    case NOP: {
      break;
    }

    case ADD: {
      std::tie(a_latch, b_latch) = regfile.foo(s, t, 0, 0, false);
      itype = RRR;
      break;
    }

    case ADDI: {
      std::tie(a_latch, std::ignore) = regfile.foo(s, 0, 0, 0, false);
      b_latch = i;
      itype = RRI;
      break;
    }

    case SUB: {
      std::tie(a_latch, b_latch) = regfile.foo(s, t, 0, 0, false);
      itype = RRR;
      break;
    }

    case SUBI: {
      std::tie(a_latch, std::ignore) = regfile.foo(s, 0, 0, 0, false);
      b_latch = i;
      itype = RRI;
      break;
    }

    case J: {
      npc = a << 2; // or *4 to guarantee 4byte word aligned
      break;
    }

    case BNEZ: {
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
      itype = RRR;
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
}

void Processor::writeback()
{
  switch (itype)
  {
    case RRR: {
      uint32_t d = (oreg >> (32 - 21)) & 0x1f;
      regfile.foo(0, 0, d, t_latch, true);
      break;
    }

    case RRI: {
      uint32_t t = (oreg >> (32 - 16)) & 0x1f;
      regfile.foo(0, 0, t, t_latch, true);
      break;
    }

    default: {
      std::cerr << "*** invalid itype ***\n";
      exit(EXIT_FAILURE);
    }
  }

  pc = npc;
}
