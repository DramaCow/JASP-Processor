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

// ==========================
// === PIPELINE FUNCTIONS ===
// ==========================

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
  execute(n_cpu);
  writeback(n_cpu);
  commit(n_cpu);
}

void Processor::fetch(Processor &n_cpu)
{
  n_cpu.ibuf = icache[this->pc];
}

void Processor::decode(Processor &n_cpu)
{
  std::string opcode = this->ibuf.opcode;

  Shelf shelf;
  shelf.opcode = opcode;

  if      ( opcode == "add" ||
            opcode == "sub" ||
            opcode == "xor"    )
  {
    int rs1 = this->rat.read(this->ibuf.params[1]);
    int rs2 = this->rat.read(this->ibuf.params[2]);
    int rd  = this->rat.alloc(this->ibuf.params[0]);
    
    n_cpu.rrf.reset(rd); // mark rd as unavailable

    shelf.dest = rd;
    std::tie(shelf.o1, shelf.v1) = rrf.read(rs1);
    std::tie(shelf.o2, shelf.v2) = rrf.read(rs2);

    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "addi" ||
            opcode == "subi"    )
  {
    int rs1 = this->rat.read(this->ibuf.params[1]);
    int o2 = this->ibuf.params[2];
    int rd  = this->rat.alloc(this->ibuf.params[0]);
    
    n_cpu.rrf.reset(rd); // mark rd as unavailable

    shelf.dest = rd;
    std::tie(shelf.o1, shelf.v1) = rrf.read(rs1);
    std::tie(shelf.o2, shelf.v2) = std::make_tuple(o2, true);

    n_cpu.rs.issue(shelf);
  }
}

void Processor::execute(Processor &n_cpu)
{
  // dispatch when instruction has finished
  if (this->alu1.duration == 0)
  {
    Shelf e = rs.dispatch(n_cpu.rs); // this may be nop
    n_cpu.alu1.dispatch(e.opcode, e.o1, e.o2, e.dest);
  }

  // and execute if instruction hasn't finished
  if (this->alu1.duration > 0)
  {
    this->alu1.execute(n_cpu.alu1);
  }
}

void Processor::writeback(Processor &n_cpu)
{
  // writeback only when flag is set and instruction has finished
  if (this->alu1.writeback && this->alu1.duration == 0)
  {
    n_cpu.rrf.write(this->alu1.dest, this->alu1.result);
    n_cpu.rs.update(this->alu1.dest, this->alu1.result);
  }
}

void Processor::commit(Processor &n_cpu)
{
}

// ========================
// === HELPER FUNCTIONS ===
// ========================

std::tuple<int, bool> Processor::read(int r)
{
  int addr = this->rat.read(r);

  // if is a ROB address
  if (addr >= NUM_REGISTERS)
  {
    return this->rob.read(addr);
  }
  // otherwise is an architectural address
  else
  {
    return this->rrf.read(addr);
  }
}

int Processor::alloc(int r)
{
  return 0;
}

bool Processor::isStalled()
{
  return this->rs.isFull();
}

// ============================
// === OPERATOR OVERLOADING ===
// ============================

Processor& Processor::operator=(const Processor& cpu)
{
  this->pc = cpu.pc;
  this->ibuf = cpu.ibuf;
  this->rob = cpu.rob;
  this->rrf = cpu.rrf;
  this->rs = cpu.rs;
  this->alu1 = cpu.alu1;
  //this->bu = cpu.bu;

  this->cycles = cpu.cycles;
  this->Instructions_executed = cpu.Instructions_executed;

  return *this;
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
  os << "  rob = {\n" << cpu.rob
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
