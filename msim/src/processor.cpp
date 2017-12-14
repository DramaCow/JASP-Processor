#include "processor.hpp"
#include <algorithm>
#include <iomanip>

#define HEX8 "0x"<<std::setfill('0')<< std::setw(8)<<std::hex
#define HEX2 "0x"<<std::setfill('0')<< std::setw(2)<<std::hex
#define HEX1 "0x"<<std::setfill('0')<< std::setw(1)<<std::hex
#define DEC std::dec

Processor::Processor(ICache &icache, DCache &dcache) :
  icache(icache),
  dcache(dcache),

  mu(dcache),

  cycles(0),
  instructions_executed(0),
  branch_corpred(0),
  branch_mispred(0)
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
  int pc = this->pc;
  int npc;

  //for (int i = 0; i < this->space(); ++i)
  for (int i = 0; i < FETCHRATE; ++i)
  {
    Instruction instruction = icache[pc];
    
    if (Instruction::isBrch(instruction.opcode))
    {
      bool prediction;
      std::tie(npc, prediction) = bp.predict(instruction, pc);
      instruction.params.push_back(prediction);
    }
    else 
    {
      npc = pc + 1;
    }

    n_cpu.ibuf[i] = std::make_tuple(pc, instruction);

    pc = npc;
  }

  // fill remainder with nops
  /*
  std::cout << "FOO " << this->space() << std::endl;
  for (int i = this->space(); i < FETCHRATE; ++i)
  {
    n_cpu.ibuf[i] = std::make_tuple(0, Instruction("nop"));
  }
  */

  n_cpu.pc = pc;
}

void Processor::decode(Processor &n_cpu)
{
  for (int i = 0; i < FETCHRATE; ++i)
  {
    int pc; Instruction instruction;
    std::tie(pc, instruction) = this->ibuf[i];

    std::string opcode = instruction.opcode;

    if      ( opcode == "mov" )
    {
      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, opcode, instruction.params[0], -1);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "movi" )
    {
      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(instruction.params[1], true);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, opcode, instruction.params[0], -1);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "add" ||
              opcode == "sub" ||
              opcode == "mul" ||
              opcode == "xor"    )
    {
      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o2, shelf.v2) = n_cpu.read(instruction.params[2]);
      std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, opcode, instruction.params[0], -1);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "addi" ||
              opcode == "subi" ||
              opcode == "muli"    )
    {
      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(instruction.params[2], true);
      std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, opcode, instruction.params[0], -1);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "b" )
    {
      int target = instruction.params[0];
      bool prediction = instruction.params[1];

      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(prediction, true); // prediction
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      std::tie(shelf.o3, shelf.v3) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, opcode, -1, prediction ? pc+1 : target);
 
      n_cpu.rob.set_spec(shelf.dest, false); // unconditional branches are not speculative
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "beq"  ||
              opcode == "bneq" ||
              opcode == "blt"  ||
              opcode == "ble"  ||
              opcode == "bgt"  ||
              opcode == "bge"     )
    {
      int target = instruction.params[2];
      bool prediction = instruction.params[3];

      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(prediction, true); // prediction
      std::tie(shelf.o2, shelf.v2) = n_cpu.read(instruction.params[0]);
      std::tie(shelf.o3, shelf.v3) = n_cpu.read(instruction.params[1]);
      shelf.dest = n_cpu.alloc(n_cpu, opcode, -1, prediction ? pc+1 : target);
 
      n_cpu.rob.set_spec(shelf.dest, true); // just a formality
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "lw" )
    {
      LSQ::Shelf shelf;
      shelf.type = LSQ::Shelf::LOAD;
      std::tie(shelf.w, shelf.vw) = std::make_tuple(0, true);
      std::tie(shelf.b, shelf.vb) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o, shelf.vo) = std::make_tuple(instruction.params[2], true);
      shelf.d = n_cpu.alloc(n_cpu, opcode, instruction.params[0], -1);
      shelf.seq = shelf.d;

      n_cpu.rob.set_spec(shelf.seq, false);
      n_cpu.lsq.issue(shelf, n_cpu.rob.get_tail());
    }
    else if ( opcode == "sw" )
    {
      LSQ::Shelf shelf;
      shelf.type = LSQ::Shelf::STORE;
      std::tie(shelf.w, shelf.vw) = n_cpu.read(instruction.params[0]);
      std::tie(shelf.b, shelf.vb) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o, shelf.vo) = std::make_tuple(instruction.params[2], true);
      shelf.seq = n_cpu.alloc(n_cpu, opcode, -1, -1); // simply the rob entry

      n_cpu.rob.set_spec(shelf.seq, false);
      n_cpu.lsq.issue(shelf, n_cpu.rob.get_tail());
    }
    else if ( opcode == "end" )
    {
      int seq = n_cpu.alloc(n_cpu, opcode, -1, -1);
      n_cpu.rob.set_spec(seq, false);
    }
  }
}

void Processor::execute(Processor &n_cpu)
{
  // determine which ports are available
  std::array<bool,NUM_EUS> port;
  bool portm;

  for (std::size_t p = 0; p < NUM_ALUS; ++p)
  {
    port[p] = this->alu[p].duration == 0;
  }
  port[NUM_EUS-1] = true; // bu port
  portm = this->mu.duration == 0;

  // receive instructions to pass to execution units
  std::array<RS::Shelf,NUM_EUS> e = this->rs.dispatch(n_cpu.rs, port);
  LSQ::Shelf em = this->lsq.dispatch(n_cpu.lsq, portm);

  // dispatch when instruction has finished
  for (std::size_t p = 0; p < NUM_ALUS; ++p)
  {
    if (port[p]) n_cpu.alu[p].dispatch(e[p].opcode, e[p].o1, e[p].o2, e[p].dest);
  }
  if (port[NUM_EUS-1]) n_cpu.bu.dispatch(e[NUM_EUS-1].opcode, e[NUM_EUS-1].o1, e[NUM_EUS-1].o2, e[NUM_EUS-1].o3, e[NUM_EUS-1].dest);
  if (portm) n_cpu.mu.dispatch(em);

  // execute if instructions haven't finished
  for (std::size_t p = 0; p < NUM_ALUS; ++p)
  {
    if (!port[p]) this->alu[p].execute(n_cpu.alu[p]);
  }
  if (!portm) this->mu.execute(n_cpu.mu);

  // === BYPASS ===

  for (int i = 0; i < NUM_ALUS; ++i)
  {
    if (n_cpu.alu[i].writeback && n_cpu.alu[i].duration == 0)
    {
      n_cpu.rs.update(n_cpu.alu[i].dest, n_cpu.alu[i].result);
      n_cpu.lsq.update(n_cpu.alu[i].dest, n_cpu.alu[i].result);
    }
  }

  if (n_cpu.mu.writeback && n_cpu.mu.duration == 0)
  {
    n_cpu.rob.write(n_cpu.mu.shelf.seq, n_cpu.mu.result);
    n_cpu.rs.update(n_cpu.mu.shelf.seq, n_cpu.mu.result);
    n_cpu.lsq.update(n_cpu.mu.shelf.seq, n_cpu.mu.result);
    n_cpu.lsq.retire(n_cpu.mu.shelf.seq);
  }
}

void Processor::writeback(Processor &n_cpu)
{
  // writeback only when flag is set and instruction has finished
  for (int i = 0; i < NUM_ALUS; ++i)
  {
    if (this->alu[i].writeback && this->alu[i].duration == 0)
    {
      n_cpu.rob.write(this->alu[i].dest, this->alu[i].result);
      n_cpu.rs.update(this->alu[i].dest, this->alu[i].result);
      n_cpu.lsq.update(this->alu[i].dest, this->alu[i].result);
    }
  }

  if (this->bu.writeback)
  {
    n_cpu.rob.write(this->bu.dest, this->bu.result);
  }

  if (this->mu.writeback && this->mu.duration == 0)
  {
    n_cpu.rob.write(this->mu.shelf.seq, this->mu.result);
    n_cpu.rs.update(this->mu.shelf.seq, this->mu.result);
    n_cpu.lsq.update(this->mu.shelf.seq, this->mu.result);
  }
}

bool Processor::commit(Processor &n_cpu)
{
  std::vector<std::tuple<int,ROB::Entry>> commits = this->rob.pop(n_cpu.rob, n_cpu.lsq);
  if (commits.size() == 0)
  {
    return false;
  }

  int idx;
  ROB::Entry entry;

  std::size_t i;
  for (i = 0; i < commits.size(); ++i)
  {
    std::tie(idx, entry) = commits[i];

    // writeback to rrf
    if (entry.type == ROB::Entry::WB)
    {
      n_cpu.rrf.write(entry.reg, entry.val);
      // NOTE: The rat entry should be freed IFF
      //       the rat entry points to the rob entry.
      //       i.e. the rob entry was the "most up to date"
      //            value for the rat entry.
      //       Otherwise the most "up to date" is held else-
      //       where, such as a different rob entry, and
      //       should NOT be modified.
      //if (this->rat.read(entry.reg) == idx)
      if (n_cpu.rat.read(entry.reg) == idx)
      {
        this->rat.write(n_cpu.rat, entry.reg, entry.reg);
      }

//      n_cpu.exe.push_back(entry.instruction); // debug
    }
    // branch
    else if (entry.type == ROB::Entry::BR)
    {
//      n_cpu.exe.push_back(entry.instruction); // debug

      if (entry.val)
      {
        n_cpu.branch_mispred = this->branch_mispred + 1;
        n_cpu.flush(entry.target);
        break; // following commits refer to mispredicts, so stop
      }
      n_cpu.branch_corpred = this->branch_corpred + 1;
    }
    else if (entry.type == ROB::Entry::SR)
    {
//      n_cpu.exe.push_back(entry.instruction); // debug
    }
    // end of program
    else if (entry.type == ROB::Entry::END)
    {
      n_cpu.instructions_executed = this->instructions_executed + i;
//      n_cpu.exe.push_back(entry.instruction); // debug
      return true; // TODO: not counted as an instruction?
    }
  }

  n_cpu.instructions_executed = this->instructions_executed + i;
  std::cout << "BAR " << i << std::endl;
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
  //int a = this->rob.push(n_cpu.rob, opcode, r, target);
  int a = n_cpu.rob.push(n_cpu.rob, opcode, r, target);
  this->rat.write(n_cpu.rat, r, a);
  return a;
}

bool Processor::isStalled()
{
  //return this->rs.isFull() || this->lsq.isFull() || this->rob.isFull();
  return this->rs.space() < FETCHRATE || this->lsq.space() < FETCHRATE || this->rob.space() < FETCHRATE;
}

int Processor::space()
{
  int count = 0;
  int abc = 0;
  int lsc = 0;

  for (int i = 0; i < FETCHRATE; ++i)
  {
    int pc; Instruction instruction;
    std::tie(pc, instruction) = this->ibuf[i];
    std::string opcode = instruction.opcode;

    if (Instruction::isArth(opcode) || Instruction::isBrch(opcode))
    {
      abc++;
    }
    else if (Instruction::isLdsr(opcode))
    {
      lsc++;
    }
    count++;
  }

  int robspace = std::max(this->rob.space()-1-count, 0);
  int rsspace  = std::max(this->rs.space()-abc, 0);
  int lsqspace = std::max(this->lsq.space()-lsc, 0);

  std::cout << "SPACE: " << robspace << ' ' << rsspace << ' ' << lsqspace << std::endl;

  return std::min({robspace, rsspace,lsqspace, FETCHRATE});
}

void Processor::flush(int target)
{
  std::cout << "=========================" << std::endl;
  std::cout << "=== MISPREDICT BRANCH ===" << std::endl;
  std::cout << "=========================" << std::endl;

  // flush and jump to entry.target
  for (int i = 0; i < FETCHRATE; ++i)
  {
    this->ibuf[i] = std::make_tuple(0, Instruction("nop"));
  }
  this->rat.reset();
  this->rob.reset();
  this->rs.reset();
  this->lsq.reset();
  for (int i = 0; i < NUM_ALUS; ++i)
  {
    this->alu[i].reset();
  }
  this->bu.reset();
  this->mu.reset();
  this->pc = target;
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
  this->lsq = cpu.lsq;
  for (int i = 0; i < NUM_ALUS; ++i)
  {
    this->alu[i] = cpu.alu[i];
  }
  this->bu = cpu.bu;
  this->mu = cpu.mu;

  this->cycles = cpu.cycles;
  this->instructions_executed = cpu.instructions_executed;
  this->branch_corpred = cpu.branch_corpred;
  this->branch_mispred = cpu.branch_mispred;

  return *this;
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
#ifdef DEBUG
  os << "{\n";
  os << "  pc = " << cpu.pc << '\n';
  os << "  ibuf = {\n";
  for (int i = 0; i < FETCHRATE; ++i)
  {
    int pc; Instruction instruction;
    std::tie(pc, instruction) = cpu.ibuf[i];
    os << "    " << pc << ": " << instruction << '\n';
  }
  os << "  }\n";
  os << "  rat = {\n"
     << "    " << cpu.rat << '\n'
     << "  }\n";
  os << "  rob = {\n" << cpu.rob
     << "  }\n";
  os << "  rrf = {\n    " << cpu.rrf << '\n';
  os << "  }\n";
  os << "  rs = {\n" << cpu.rs
     << "  }\n";
  os << "  lsq = {\n" << cpu.lsq
     << "  }\n";
  for (int i = 0; i < NUM_ALUS; ++i)
  {
    os << "  alu[" << i << "] = {\n" << cpu.alu[i]
       << "  }\n";
  }
  os << "  bu = {\n" << cpu.bu
     << "  }\n";
  os << "  mu = {\n" << cpu.mu
     << "  }\n";
  os << "  dcache = {\n" << cpu.dcache
     << "  }\n";
  os << "}\n";
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     //<< "instructions_executed = " << cpu.instructions_executed << '\n'
     << "cpi = " << ((double)cpu.cycles / (double)cpu.instructions_executed) << '\n'
     << "ipc = " << ((double)cpu.instructions_executed / (double)cpu.cycles) << '\n'
     << "branch pred rate = " << (double)cpu.branch_corpred / (double)(cpu.branch_corpred + cpu.branch_mispred);
#else
  os << cpu.rrf;
#endif
  return os;
}
