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

// ==========================
// === PIPELINE FUNCTIONS ===
// ==========================

bool Processor::tick(Processor &n_cpu)
{
  n_cpu.cycles++;
  if (!isStalled())
  {
    fetch(n_cpu);
    decode(n_cpu);
  }
  execute(n_cpu);
  writeback(n_cpu);
  return commit(n_cpu);
}

void Processor::fetch(Processor &n_cpu)
{
  Instruction instruction = icache[this->pc];

  if (Instruction::isBrch(instruction.opcode))
  {
    bool prediction;
    std::tie(n_cpu.pc, prediction) = bp.predict(instruction, this->pc);
    instruction.params.push_back(prediction);
  }
  else 
  {
    n_cpu.pc = this->pc + 1;
  }

  n_cpu.ibuf = std::make_tuple(this->pc, instruction);
}

void Processor::decode(Processor &n_cpu)
{
  int pc; Instruction instruction;
  std::tie(pc, instruction) = this->ibuf;

  std::string opcode = instruction.opcode;

  Shelf shelf;
  shelf.opcode = opcode;

  if      ( opcode == "mov" )
  {
    std::tie(shelf.o1, shelf.v1) = this->read(instruction.params[1]);
    std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
    std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
    shelf.dest = this->alloc(n_cpu, opcode, instruction.params[0], -1);

    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "movi" )
  {
    std::tie(shelf.o1, shelf.v1) = std::make_tuple(instruction.params[1], true);
    std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
    std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
    shelf.dest = this->alloc(n_cpu, opcode, instruction.params[0], -1);

    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "add" ||
            opcode == "sub" ||
            opcode == "mul" ||
            opcode == "xor"    )
  {
    std::tie(shelf.o1, shelf.v1) = this->read(instruction.params[1]);
    std::tie(shelf.o2, shelf.v2) = this->read(instruction.params[2]);
    std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
    shelf.dest = this->alloc(n_cpu, opcode, instruction.params[0], -1);

    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "addi" ||
            opcode == "subi" ||
            opcode == "muli"    )
  {
    std::tie(shelf.o1, shelf.v1) = this->read(instruction.params[1]);
    std::tie(shelf.o2, shelf.v2) = std::make_tuple(instruction.params[2], true);
    std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
    shelf.dest = this->alloc(n_cpu, opcode, instruction.params[0], -1);

    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "b" )
  {
    int target = instruction.params[0];
    bool prediction = instruction.params[1];

    std::tie(shelf.o1, shelf.v1) = std::make_tuple(prediction, true); // prediction
    std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
    std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
    shelf.dest = this->alloc(n_cpu, opcode, -1, prediction ? pc+1 : target);
 
    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "beq" ||
            opcode == "bneq"   )
  {
    int target = instruction.params[2];
    bool prediction = instruction.params[3];

    std::tie(shelf.o1, shelf.v1) = std::make_tuple(prediction, true); // prediction
    std::tie(shelf.o2, shelf.v2) = this->read(instruction.params[0]);
    std::tie(shelf.o3, shelf.v3) = this->read(instruction.params[1]);
    shelf.dest = this->alloc(n_cpu, opcode, -1, prediction ? pc+1 : target);
 
    n_cpu.rs.issue(shelf);
  }
  else if ( opcode == "end" )
  {
    this->alloc(n_cpu, opcode, -1, 0);
  }
}

void Processor::execute(Processor &n_cpu)
{
  // determine which ports are available
  bool port1 = this->alu1.duration == 0;
  bool port2 = true;

  Shelf e1, e2;
  std::tie(e1, e2) = rs.dispatch(n_cpu.rs, port1, port2);

  // dispatch when instruction has finished
  if (port1) n_cpu.alu1.dispatch(e1.opcode, e1.o1, e1.o2, e1.dest);
  if (port2) n_cpu.bu.dispatch(e2.opcode, e2.o1, e2.o2, e2.o3, e2.dest);

  // execute if instructions haven't finished
  if (!port1) this->alu1.execute(n_cpu.alu1);

  // === BYPASS ===

  if (n_cpu.alu1.writeback && n_cpu.alu1.duration == 0)
  {
    n_cpu.rs.update(n_cpu.alu1.dest, n_cpu.alu1.result);
  }
}

void Processor::writeback(Processor &n_cpu)
{
  // writeback only when flag is set and instruction has finished
  if (this->alu1.writeback && this->alu1.duration == 0)
  {
    n_cpu.rob.write(this->alu1.dest, this->alu1.result);
    n_cpu.rs.update(this->alu1.dest, this->alu1.result);
  }

  if (this->bu.writeback)
  {
    n_cpu.rob.write(this->bu.dest, this->bu.result);
  }
}

bool Processor::commit(Processor &n_cpu)
{
  std::vector<std::tuple<int,ROB::ROBEntry>> commits = this->rob.pop(n_cpu.rob);
  if (commits.size() == 0)
  {
    return false;
  }

  int idx;
  ROB::ROBEntry entry;

  std::size_t i;
  for (i = 0; i < commits.size(); ++i)
  {
    std::tie(idx, entry) = commits[i];

    // writeback to rrf
    if (entry.type == ROB::ROBEntry::WB)
    {
      n_cpu.rrf.write(entry.reg, entry.val);
      // NOTE: The rat entry should be freed IFF
      //       the rat entry points to the rob entry.
      //       i.e. the rob entry was the "most up to date"
      //            value for the rat entry.
      //       Otherwise the most "up to date" is held else-
      //       where, such as a different rob entry, and
      //       should NOT be modified.
      if (this->rat.read(entry.reg) == idx)
      {
        this->rat.write(n_cpu.rat, entry.reg, entry.reg);
      }
    }
    // branch
    else if (entry.type == ROB::ROBEntry::BR)
    {
      if (entry.val)
      {
        // flush and jump to entry.target
        n_cpu.ibuf = std::make_tuple(0, Instruction("nop"));
        n_cpu.rat.reset();
        n_cpu.rob.reset();
        n_cpu.rs.reset();
        n_cpu.alu1.reset();
        n_cpu.bu.reset();
        n_cpu.pc = entry.target;
        break; // following commits refer to mispredicts, so stop
      }
    }
    // end of program
    else if (entry.type == ROB::ROBEntry::END)
    {
      return true; // TODO: not counted as an instruction?
    }
  }

  n_cpu.instructions_executed = this->instructions_executed + i;
  return false;
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

int Processor::alloc(Processor &n_cpu, std::string opcode, int r, int target)
{
  int a = this->rob.push(n_cpu.rob, opcode, r, target);
  this->rat.write(n_cpu.rat, r, a);
  return a;
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
  this->bp = cpu.bp; // TODO: does this need to be copied (does it even need to be an object)
  this->rat = cpu.rat;
  this->rob = cpu.rob;
  this->rrf = cpu.rrf;
  this->rs = cpu.rs;
  this->alu1 = cpu.alu1;
  this->bu = cpu.bu;

  this->cycles = cpu.cycles;
  this->instructions_executed = cpu.instructions_executed;

  return *this;
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
#ifdef DEBUG
  os << "{\n";
  os << "  pc = " << cpu.pc << '\n';
  int pc; Instruction instruction;
  std::tie(pc, instruction) = cpu.ibuf;
  os << "  ibuf = {\n"
     << "    " << pc << ": " << instruction << '\n'
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
  os << "  bu = {\n" << cpu.bu
     << "  }\n";
  os << "}";
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     << "instructions_executed = " << cpu.instructions_executed << '\n'
     << "instructions_per_cycle = " << ((double)cpu.instructions_executed / (double)cpu.cycles);
#else
  os << cpu.rrf;
#endif
  return os;
}
