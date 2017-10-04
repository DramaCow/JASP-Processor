#include "processor.hpp"
#include <iomanip>

Processor::Processor(Memory &imem, Memory &dmem) :
  state(FETCH),
  imem(imem),
  dmem(dmem),
  pc(0),
  oreg(0),

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
  pc += 4;
}

void Processor::decode()
{
  uint32_t opcode = (oreg >> (32 - 6)) & 0x3f; // first 6 bits

  switch (opcode)
  {
    // ADD
    case 0: {
      uint32_t s = (oreg >> (32 - 11)) & 0x1f;
      uint32_t t = (oreg >> (32 - 16)) & 0x1f;

      std::tie(alu.areg, alu.breg) = regfile.foo(s, t, 0, 0, false);
      alu.op = ADD;

      itype = RRR;

      break;
    }

    // ADDI
    case 1: {
      uint32_t s = (oreg >> (32 - 11)) & 0x1f;
      uint32_t i = oreg & 0xffff;

      std::tie(alu.areg, std::ignore) = regfile.foo(s, 0, 0, 0, false);
      alu.breg = i;
      alu.op = ADD;

      itype = RRI;

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
  alu.execute();
}

void Processor::writeback()
{
  switch (itype)
  {
    case RRR: {
      uint32_t d = (oreg >> (32 - 21)) & 0x1f;
      regfile.foo(0, 0, d, alu.res, true);
      break;
    }

    case RRI: {
      uint32_t t = (oreg >> (32 - 16)) & 0x1f;
      regfile.foo(0, 0, t, alu.res, true);
      break;
    }

    default: {
      std::cerr << "*** invalid itype ***\n";
      exit(EXIT_FAILURE);
    }
  }
}
