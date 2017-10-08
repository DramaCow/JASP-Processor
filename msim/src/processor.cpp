#include "processor.hpp"
#include "isa.hpp"
#include <iomanip>

Processor::Processor(Memory &imem, Memory &dmem) :
  state(0),

  imem(imem),
  dmem(dmem),

  cycles(0),
  instructions_executed(0)
{
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << "{\n"
     << "  pc = " << std::dec << cpu.address.pc << '\n'
//     << "  oreg = " << std::setfill('0') << std::setw(8) << std::hex << cpu.oreg << '\n'
//     << "  a_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.a_latch << '\n'
//     << "  b_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.b_latch << '\n'
//     << "  t_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.t_latch << '\n'
//     << "  c_latch = " << std::setfill('0') << std::setw(8) << std::hex << cpu.c_latch << '\n'
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
    case 3: os << "acessing memory..."; break;
    case 4: os << "writing..."; break;
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
    case 3/*MEMACCESS*/: memaccess(); break;
    case 4/*WRITEBACK*/: writeback(); break;
    default: {
      std::cerr << "*** entered invalid pipeline state ***\n";
      exit(EXIT_FAILURE);
    }
  }
  state = (state + 1) % 5;
  cycles++;
}

void Processor::fetch()
{
  uint32_t npc = address.pc + 4;
  uint32_t oreg = imem[address.pc];

  lat_f_d.npc  = npc;
  lat_f_d.oreg = oreg;
}

void Processor::decode()
{
  uint32_t opcode = (lat_f_d.oreg >> (32 - 6)) & 0x3f; // first 6 bits
  uint32_t rs  = (lat_f_d.oreg >> (32 - 11)) & 0x1f; // next 5
  uint32_t rt  = (lat_f_d.oreg >> (32 - 16)) & 0x1f; // next 5
  uint32_t rd  = (lat_f_d.oreg >> (32 - 21)) & 0x1f; // next 5
  uint32_t imm = lat_f_d.oreg & 0xffff; // last 16

  uint32_t a, b;
  std::tie(a, b) = regfile.foo(rs, rt, 0, 0, false);
  uint32_t rdest = rd;
  if (opcode == ADDI || opcode == SUBI || opcode == LDI) {
    rdest = rt;
  }

  lat_d_e.npc    = lat_f_d.npc;
  lat_d_e.opcode = opcode;
  lat_d_e.a      = a;
  lat_d_e.b      = b;
  lat_d_e.imm    = imm;
  lat_d_e.rdest  = rdest;
}

void Processor::execute()
{
  uint32_t t = 0; // can be whatever for operations that don't use it

  switch (lat_d_e.opcode)
  {
    case NOP: {
      break;
    }
    case ADD: {
      t = lat_d_e.a + lat_d_e.b;
      break;
    }
    case ADDI: {
      t = lat_d_e.a + lat_d_e.imm;
      break;
    }
    case SUB: {
      t = lat_d_e.a - lat_d_e.b;
      break;
    }
    case SUBI: {
      t = lat_d_e.a - lat_d_e.imm;
      break;
    }
    case J: {
      t = 0 + lat_d_e.imm;
      break;
    }
    case JNEZ: {
      t = 0 + lat_d_e.imm;
      break;
    }
    case LDI: {
      t = lat_d_e.a + lat_d_e.imm;
      break;
    }
    case STI: {
      t = lat_d_e.a + lat_d_e.imm;
      break;
    }
    case XOR: {
      t = lat_d_e.a ^ lat_d_e.b;
      break;
    }
    default: {
      std::cerr << "*** invalid operation ***\n";
      exit(EXIT_FAILURE);
    }
  }

  uint32_t cmp =
//    (((lat_d_e.a  > 0u) << 2) & 0x4) |
//    (((lat_d_e.a  < 0u) << 1) & 0x2) |
    (((lat_d_e.a == 0u) << 0) & 0x1);

  lat_e_m.npc    = lat_d_e.npc;
  lat_e_m.opcode = lat_d_e.opcode;
  lat_e_m.cmp    = cmp;
  lat_e_m.t      = t;
  lat_e_m.b      = lat_d_e.b;
  lat_e_m.rdest  = lat_d_e.rdest;

  instructions_executed++;
}

void Processor::memaccess()
{
  uint32_t npc = lat_e_m.npc;
  uint32_t data = lat_e_m.t;

  switch (lat_e_m.opcode)
  {
    case J: {
      npc = lat_e_m.t << 2;
    }
    case JNEZ: {
      if ((lat_e_m.cmp & 0x1) == 0)
      {
        npc = lat_e_m.t << 2; 
      }
      break;
    }
  }

  // access memory here
  switch (lat_e_m.opcode)
  {
    case LDI: data = dmem[lat_e_m.t << 2]; break;
    case STI: {
      uint8_t b[4];
      b[0] = (lat_e_m.b >>  0) & 0xff; 
      b[1] = (lat_e_m.b >>  8) & 0xff; 
      b[2] = (lat_e_m.b >> 16) & 0xff; 
      b[3] = (lat_e_m.b >> 24) & 0xff; 
      dmem.copy(lat_e_m.t << 2, b, 4);
      break;
    }
  }

  lat_m_w.npc    = npc; // can instead be directly moved to address? (see diagram) 
  lat_m_w.opcode = lat_e_m.opcode;
  lat_m_w.data   = data;
  lat_m_w.rdest  = lat_e_m.rdest;
}

void Processor::writeback()
{
  bool we = false; // write enabled

  switch (lat_m_w.opcode)
  {
    case ADD:  we = true; break;
    case ADDI: we = true; break;
    case SUB:  we = true; break;
    case SUBI: we = true; break;
    case LDI:  we = true; break;
    case XOR:  we = true; break;
  }

  regfile.foo(0, 0, lat_m_w.rdest, lat_m_w.data, we);

  address.pc = lat_m_w.npc;
}
