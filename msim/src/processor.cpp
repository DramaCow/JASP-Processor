#include "processor.hpp"
#include "isa.hpp"
#include <iomanip>

#define HEX8 "0x"<<std::setfill('0')<< std::setw(8)<<std::hex
#define HEX2 "0x"<<std::setfill('0')<< std::setw(2)<<std::hex
#define HEX1 "0x"<<std::setfill('0')<< std::setw(1)<<std::hex
#define DEC std::dec

Processor::Processor(ICache &icache, DCache &dcache) :
  icache(icache),
  dcache(dcache),

  cycles(0),
  instructions_executed(0)
{
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << "{\n";
/*
  os << "  address = {\n"
     << "    pc = " << cpu.address.pc << '\n'
     << "  }\n";
  os << "  Lat_f_d = {\n"
     << "    pc = " << cpu.lat_f_d.pc << '\n'
     << "    instruction = " << cpu.lat_f_d.instruction << '\n'
     << "  }\n";
  os << "  Lat_d_e = {\n"
     << "    pc = " << cpu.lat_d_e.pc << '\n'
		 << "    we = " << cpu.lat_d_e.we << '\n'
     << "  }\n";
  os << "  Lat_e_m = {\n"
     << "    pc = " << cpu.lat_e_m.pc << '\n'
     << "    result = " << cpu.lat_e_m.result << '\n'
     << "    rd = " << cpu.lat_e_m.rd << '\n'
     << "    we = " << cpu.lat_e_m.we << '\n'
     << "  }\n";
  os << "  Lat_m_w = {\n"
     << "    pc = " << cpu.lat_m_w.pc << '\n'
     << "    rd = " << cpu.lat_m_w.rd << '\n'
     << "    data = " << cpu.lat_m_w.data << '\n'
     << "    we = " << cpu.lat_m_w.we << '\n'
     << "  }\n";
*/
  os << "  regfile = \n    " << cpu.regfile << '\n';
  os << "  restat = \n" << cpu.restat;
  os << "}";
/*
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     << "instructions_executed = " << cpu.instructions_executed << '\n'
     << "instructions_per_cycle = " << ((double)cpu.instructions_executed / (double)cpu.cycles);
*/
  return os;
}

void Processor::tick(Processor &n_cpu)
{
  fetch(n_cpu);
  decode(n_cpu);
  execute(n_cpu);
  memaccess(n_cpu);
  writeback(n_cpu);
}

Processor& Processor::operator=(const Processor& cpu)
{
  this->regfile = cpu.regfile;
  this->restat = cpu.restat;
  this->address = cpu.address;
  this->lat_f_d = cpu.lat_f_d;
  this->lat_d_e = cpu.lat_d_e;
  this->lat_e_m = cpu.lat_e_m;
  this->lat_m_w = cpu.lat_m_w;
  this->cycles = cpu.cycles;
  this->instructions_executed = cpu.instructions_executed;
  return *this;
}

void Processor::fetch(Processor &n_cpu)
{
  Instruction instruction = icache[address.pc];
  unsigned int pc = address.pc + 1;

  n_cpu.lat_f_d.pc = pc;
  n_cpu.lat_f_d.instruction = instruction;
}

void Processor::decode(Processor &n_cpu)
{
  std::string opcode = lat_f_d.instruction.opcode;
  std::cout << opcode << std::endl;
  if (lat_f_d.instruction.params.size() >= 3)
  {
    int os1; bool v1;
    int os2; bool v2;

    int rd  = lat_f_d.instruction.params[0];

    int rs1 = lat_f_d.instruction.params[1];
    std::tie(os1, v1) = regfile.read(rs1);

    if (opcode == "addi" || opcode == "subi")
    {
      os2 = lat_f_d.instruction.params[2];
      v2 = true;
    }
    else
    {
      int rs2 = lat_f_d.instruction.params[2];
      std::tie(os2, v2) = regfile.read(rs2);
    }

    n_cpu.restat.insert(opcode, os1, os2, rd);
  }
  bool we = false;
  if (opcode == "add" || opcode == "addi" || opcode == "sub" || opcode == "subi" || opcode == "xor")
  {
    we = true;
  }

  n_cpu.lat_d_e.pc = lat_f_d.pc;
  n_cpu.lat_d_e.we = we;
}

void Processor::execute(Processor &n_cpu)
{
  Entry e = restat.dispatch(n_cpu.restat);
  int result;
  if (e.opcode == "add" || e.opcode == "addi")
  {
    result = e.os1 + e.os2;
  }
  else if (e.opcode == "sub" || e.opcode == "subi")
  {
    result = e.os1 - e.os2;
  }
  else if (e.opcode == "xor")
  {
    result = e.os1 ^ e.os2;
  }

  n_cpu.lat_e_m.pc = lat_d_e.pc;
  n_cpu.lat_e_m.result = result;
  n_cpu.lat_e_m.rd = e.rd;
  n_cpu.lat_e_m.we = lat_d_e.we;
}

void Processor::memaccess(Processor &n_cpu)
{
  n_cpu.lat_m_w.pc = lat_e_m.pc;
  n_cpu.lat_m_w.data = lat_e_m.result;
  n_cpu.lat_m_w.rd = lat_e_m.rd;
  n_cpu.lat_m_w.we = lat_e_m.we;
}

void Processor::writeback(Processor &n_cpu)
{
  if (lat_m_w.we)
  {
    n_cpu.regfile.write(lat_m_w.rd, lat_m_w.data);
  }
  n_cpu.address.pc = lat_m_w.pc;
}
