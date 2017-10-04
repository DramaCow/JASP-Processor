#include "processor.hpp"

Processor::Processor(Memory &imem, Memory &dmem) :
  state(FETCH),
  imem(imem),
  dmem(dmem),
  pc(0),
  oreg(0)
{
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << cpu.regfile;
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
      break;
    }

    case WRITEBACK: {
      writeback(); 
      state = FETCH;
      break;
    }

    default: {
      // TODO: error message
      exit(EXIT_FAILURE);
    }
  }
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

      break;
    }

    default: {
      // TODO: error message
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
  uint32_t d = (oreg >> (32 - 21)) & 0x1f;
  regfile.foo(0, 0, d, alu.res, true);
}
