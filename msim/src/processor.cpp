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
  //this->bu = cpu.bu;

  this->cycles = cpu.cycles;
  this->Instructions_executed = cpu.Instructions_executed;

  return *this;
}

void Processor::fetch(Processor &n_cpu)
{
  n_cpu.ibuf = icache[this->pc];
}

void Processor::decode(Processor &n_cpu)
{
  std::string opcode = this->ibuf.opcode;

  Entry entry;
  entry.opcode = opcode;

  if (opcode == "nop")
  {
    return;
  }
  else if ( opcode == "add" ||
            opcode == "sub" ||
            opcode == "xor"    )
  {
    int rs1 = this->rat.read(this->ibuf.params[1]);
    int rs2 = this->rat.read(this->ibuf.params[2]);
    int rd  = this->rat.alloc(this->ibuf.params[0]);
    
    n_cpu.rrf.reset(rd); // mark rd as unavailable

    entry.dest = rd;
    std::tie(entry.o1, entry.v1) = rrf.read(rs1);
    std::tie(entry.o2, entry.v2) = rrf.read(rs2);
  }
  else if ( opcode == "addi" ||
            opcode == "subi"    )
  {
    int rs1 = this->rat.read(this->ibuf.params[1]);
    int os2 = this->ibuf.params[2];
    int rd  = this->rat.alloc(this->ibuf.params[0]);
    
    n_cpu.rrf.reset(rd); // mark rd as unavailable

    entry.dest = rd;
    std::tie(entry.o1, entry.v1) = rrf.read(rs1);
    std::tie(entry.o2, entry.v2) = std::make_tuple(os2, true);
  }

  n_cpu.rs.issue(entry);
}

void Processor::dispatch(Processor &n_cpu)
{
  // about to finish executing current Instruction
  if (this->alu1.duration <= 1)
  {
    Entry e = rs.dispatch(n_cpu.rs);
    n_cpu.alu1.dispatch(e.opcode, e.o1, e.o2, e.dest); // next state stores Instruction
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
