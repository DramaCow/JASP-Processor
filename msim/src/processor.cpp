#include "processor.hpp"
#include "isa.hpp"
#include <iomanip>

#define HEX8 "0x"<<std::setfill('0')<< std::setw(8)<<std::hex
#define HEX2 "0x"<<std::setfill('0')<< std::setw(2)<<std::hex
#define HEX1 "0x"<<std::setfill('0')<< std::setw(1)<<std::hex
#define DEC std::dec

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
     << "  address = {\n"
     << "    pc = " << DEC << cpu.address.pc << '\n'
     << "  }\n" 
     << "  Lat_f_d = {\n"
     << "    npc = " << DEC << cpu.lat_f_d.npc << '\n'
     << "    oreg = " << HEX8 << cpu.lat_f_d.oreg << '\n'
     << "  }\n" 
     << "  Lat_d_e = {\n"
     << "    npc = " << DEC << cpu.lat_d_e.npc << '\n'
     << "    opcode = " << HEX2 << cpu.lat_d_e.opcode << '\n'
     << "    a = " << DEC << cpu.lat_d_e.a << '\n'
     << "    b = " << DEC << cpu.lat_d_e.b << '\n'
     << "    imm = " << DEC << cpu.lat_d_e.imm << '\n'
     << "    rdest = " << HEX2 << cpu.lat_d_e.rdest << '\n'
     << "  }\n" 
     << "  Lat_e_m = {\n"
     << "    npc = " << DEC << cpu.lat_e_m.npc << '\n'
     << "    opcode = " << HEX2 << cpu.lat_e_m.opcode << '\n'
     << "    cmp = " << HEX1 << cpu.lat_e_m.cmp << '\n'
     << "    t = " << DEC << cpu.lat_e_m.t << '\n'
     << "    b = " << DEC << cpu.lat_e_m.b << '\n'
     << "    rdest = " << HEX2 << cpu.lat_e_m.rdest << '\n'
     << "  }\n" 
     << "  Lat_m_w = {\n"
     << "    npc = " << DEC << cpu.lat_m_w.npc << '\n'
     << "    opcode = " << HEX2 << cpu.lat_m_w.opcode << '\n'
     << "    data = " << DEC << cpu.lat_m_w.data << '\n'
     << "    rdest = " << HEX2 << cpu.lat_m_w.rdest << '\n'
     << "  }\n" 
     << "  regfile = \n    " << cpu.regfile << '\n'
     << "}\n";
/*
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     << "instructions_executed = " << cpu.instructions_executed << '\n'
     << "instructions_per_cycle = " << ((double)cpu.instructions_executed / (double)cpu.cycles);
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

void Processor::tick(Processor &n_cpu)
{
  switch (state)
  {
    case 0/*FETCH*/:     fetch(n_cpu);     break;
    case 1/*DECODE*/:    decode(n_cpu);    break;
    case 2/*EXECUTE*/:   execute(n_cpu);   break;
    case 3/*MEMACCESS*/: memaccess(n_cpu); break;
    case 4/*WRITEBACK*/: writeback(n_cpu); break;
    default: {
      std::cerr << "*** entered invalid pipeline state ***\n";
      exit(EXIT_FAILURE);
    }
  }
  state = (state + 1) % 5;
  cycles++;
}

void Processor::fetch(Processor &n_cpu)
{
  uint32_t npc = address.pc + 4;
  uint32_t oreg = imem[address.pc];

  n_cpu.lat_f_d.npc  = npc;
  n_cpu.lat_f_d.oreg = oreg;
}

void Processor::decode(Processor &n_cpu)
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

  n_cpu.lat_d_e.npc    = lat_f_d.npc;
  n_cpu.lat_d_e.opcode = opcode;
  n_cpu.lat_d_e.a      = a;
  n_cpu.lat_d_e.b      = b;
  n_cpu.lat_d_e.imm    = imm;
  n_cpu.lat_d_e.rdest  = rdest;
}

void Processor::execute(Processor &n_cpu)
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

  n_cpu.lat_e_m.npc    = lat_d_e.npc;
  n_cpu.lat_e_m.opcode = lat_d_e.opcode;
  n_cpu.lat_e_m.cmp    = cmp;
  n_cpu.lat_e_m.t      = t;
  n_cpu.lat_e_m.b      = lat_d_e.b;
  n_cpu.lat_e_m.rdest  = lat_d_e.rdest;

  instructions_executed++;
}

void Processor::memaccess(Processor &n_cpu)
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

  n_cpu.lat_m_w.npc    = npc; // can instead be directly moved to address? (see diagram) 
  n_cpu.lat_m_w.opcode = lat_e_m.opcode;
  n_cpu.lat_m_w.data   = data;
  n_cpu.lat_m_w.rdest  = lat_e_m.rdest;
}

void Processor::writeback(Processor &n_cpu)
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

  n_cpu.address.pc = lat_m_w.npc;
}
