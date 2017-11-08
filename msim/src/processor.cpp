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
  os << "  alu1 = {\n" << cpu.alu1
     << "  }\n";
  os << "}";
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     << "instructions_executed = " << cpu.instructions_executed << '\n'
     << "instructions_per_cycle = " << ((double)cpu.instructions_executed / (double)cpu.cycles);
  return os;
}

void Processor::tick(Processor &n_cpu)
{
  n_cpu.cycles++;
  if (!isStalled())
  {
    n_cpu.pc++;
    fetch(n_cpu);
    decode(n_cpu);
  }
  n_cpu.restat.tick(); // age all used entries
  dispatch(n_cpu);
  execute(n_cpu);
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
  this->alu1 = cpu.alu1;
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
    entry.rd = rd;
    n_cpu.restat.issue(entry);
  }
}

void Processor::dispatch(Processor &n_cpu)
{
  // about to finish executing current instruction
  if (this->alu1.duration <= 1)
  {
    Entry e = restat.dispatch(n_cpu.restat);
    n_cpu.alu1.dispatch(e.opcode, e.os1, e.os2, e.rd); // next state stores instruction
  }
}

void Processor::execute(Processor &n_cpu)
{
  n_cpu.instructions_executed += this->alu1.execute(n_cpu.alu1);
}

void Processor::writeback(Processor &n_cpu)
{
  if (this->alu1.we)
  {
    n_cpu.regfile.write(this->alu1.dest, this->alu1.result);
    n_cpu.restat.update(this->alu1.result, this->alu1.dest);
    this->alu1.we = false;
  }
}
