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
  Instructions_executed(0)
{
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << "{\n";
  os << "  pc = " << cpu.pc << '\n';
  os << "  ibuf = {\n"
     << "    " << cpu.ibuf << '\n'
     << "  }\n";
  os << "  rat = {\n"
     << "    " << cpu.rat << '\n'
     << "  }\n";
  os << "  rrf = {\n    " << cpu.rrf << '\n';
  os << "  }\n";
  os << "  rs = {\n" << cpu.rs
     << "  }\n";
  os << "  alu1 = {\n" << cpu.alu1
     << "  }\n";
//  os << "  bu = {\n" << cpu.bu
//     << "  }\n";
  os << "}";
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     << "Instructions_executed = " << cpu.Instructions_executed << '\n'
     << "Instructions_per_cycle = " << ((double)cpu.Instructions_executed / (double)cpu.cycles);
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
  n_cpu.rs.tick(); // age all used entries
  dispatch(n_cpu);
  execute(n_cpu);
  writeback(n_cpu);
  commit(n_cpu);
}

bool Processor::isStalled()
{
  return this->rs.isFull();
}

Processor& Processor::operator=(const Processor& cpu)
{
  this->pc = cpu.pc;
  this->ibuf = cpu.ibuf;
  this->rrf = cpu.rrf;
  this->rs = cpu.rs;
  this->alu1 = cpu.alu1;
  //this->BU = cpu.BU;

  this->cycles = cpu.cycles;
  this->Instructions_executed = cpu.Instructions_executed;

  return *this;
}

void Processor::fetch(Processor &n_cpu)
{
  Instruction Instruction = icache[this->pc];
  n_cpu.ibuf = Instruction;
}

void Processor::decode(Processor &n_cpu)
{
  std::string opcode = this->ibuf.opcode;
  if (this->ibuf.params.size() >= 3)
  {
    int os1; bool v1;
    int os2; bool v2;

    int rd  = this->ibuf.params[0];
    n_cpu.rrf.reset(rd);

    int rs1 = this->ibuf.params[1];
    std::tie(os1, v1) = rrf.read(rs1);

    if (opcode == "addi" || opcode == "subi")
    {
      os2 = this->ibuf.params[2];
      v2 = true;
    }
    else
    {
      int rs2 = this->ibuf.params[2];
      std::tie(os2, v2) = rrf.read(rs2);
    }

    Entry entry;
    entry.opcode = opcode;
    entry.os1 = os1; entry.v1 = v1;
    entry.os2 = os2; entry.v2 = v2;
    entry.rd = rd;
    n_cpu.rs.issue(entry);
  }
}

void Processor::dispatch(Processor &n_cpu)
{
  // about to finish executing current Instruction
  if (this->alu1.duration <= 1)
  {
    Entry e = rs.dispatch(n_cpu.rs);
    n_cpu.alu1.dispatch(e.opcode, e.os1, e.os2, e.rd); // next state stores Instruction
  }
}

void Processor::execute(Processor &n_cpu)
{
  n_cpu.Instructions_executed += this->alu1.execute(n_cpu.alu1);
}

void Processor::writeback(Processor &n_cpu)
{
  if (this->alu1.we)
  {
    n_cpu.rrf.write(this->alu1.dest, this->alu1.result);
    n_cpu.rs.update(this->alu1.result, this->alu1.dest);
    this->alu1.we = false;
  }
}

void Processor::commit(Processor &n_cpu)
{
}
