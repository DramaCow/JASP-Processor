#include "processor.hpp"
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
  os << "  pc = " << cpu.pc << '\n';
  os << "  instbuf = {\n"
     << "    " << cpu.instbuf << '\n'
     << "  }\n";
  os << "  regfile = {\n    " << cpu.regfile << '\n';
  os << "  }\n";
  os << "  restat = {\n" << cpu.restat
     << "  }\n";
  os << "  Lat_e_m = {\n"
     << "    result = " << cpu.lat_e_m.result << '\n'
     << "    rd = " << cpu.lat_e_m.rd << '\n'
     << "    we = " << cpu.lat_e_m.we << '\n'
     << "  }\n";
  os << "  Lat_m_w = {\n"
     << "    rd = " << cpu.lat_m_w.rd << '\n'
     << "    data = " << cpu.lat_m_w.data << '\n'
     << "    we = " << cpu.lat_m_w.we << '\n'
     << "  }\n";
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
  if (!this->isStalled())
  {
    n_cpu.pc++;
    fetch(n_cpu);
    decode(n_cpu);
  }
  n_cpu.restat.tick(); // age all used entries
  execute(n_cpu);
  memaccess(n_cpu);
  writeback(n_cpu);
}

bool Processor::isStalled()
{
  return this->restat.isFull();
}

Processor& Processor::operator=(const Processor& cpu)
{
  this->pc = cpu.pc;
  this->instbuf = cpu.instbuf;
  this->regfile = cpu.regfile;
  this->restat = cpu.restat;
  this->lat_e_m = cpu.lat_e_m;
  this->lat_m_w = cpu.lat_m_w;
  this->cycles = cpu.cycles;
  this->instructions_executed = cpu.instructions_executed;
  return *this;
}

void Processor::fetch(Processor &n_cpu)
{
  Instruction instruction = icache[this->pc];
  n_cpu.instbuf = instruction;
}

void Processor::decode(Processor &n_cpu)
{
  std::string opcode = this->instbuf.opcode;
  if (this->instbuf.params.size() >= 3)
  {
    int os1; bool v1;
    int os2; bool v2;

    int rd  = this->instbuf.params[0];
    n_cpu.regfile.reset(rd);

    int rs1 = this->instbuf.params[1];
    std::tie(os1, v1) = regfile.read(rs1);

    if (opcode == "addi" || opcode == "subi")
    {
      os2 = this->instbuf.params[2];
      v2 = true;
    }
    else
    {
      int rs2 = this->instbuf.params[2];
      std::tie(os2, v2) = regfile.read(rs2);
    }

    Entry entry;
    entry.opcode = opcode;
    entry.os1 = os1; entry.v1 = v1;
    entry.os2 = os2; entry.v2 = v2;
    entry.rd = rd; entry.we = true;
    n_cpu.restat.issue(entry);
  }
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

  n_cpu.lat_e_m.result = result;
  n_cpu.lat_e_m.rd = e.rd;
  n_cpu.lat_e_m.we = e.we;
}

void Processor::memaccess(Processor &n_cpu)
{
  n_cpu.lat_m_w.data = lat_e_m.result;
  n_cpu.lat_m_w.rd = lat_e_m.rd;
  n_cpu.lat_m_w.we = lat_e_m.we;
}

void Processor::writeback(Processor &n_cpu)
{
  if (lat_m_w.we)
  {
    n_cpu.regfile.write(lat_m_w.rd, lat_m_w.data);
    n_cpu.restat.update(lat_m_w.data, lat_m_w.rd); // associative update of reservation station
  }
}
