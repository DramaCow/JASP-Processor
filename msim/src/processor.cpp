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
  branch_mispred(0),
  instructions_fetched(0)
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
    n_cpu.instructions_fetched = this->instructions_fetched + FETCHRATE;
    fetch(n_cpu);
    decode(n_cpu);
  }
#ifdef DEBUG
  else
  {
    std::cout << "===============" << std::endl;
    std::cout << "=== STALLED ===" << std::endl;
    std::cout << "===============" << std::endl;
  }
#endif
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
      npc = instruction.getTakenBTA();
      instruction.params.push_back(pc+1);
      instruction.params.push_back(npc);
      //bool prediction;
      //std::tie(npc, prediction) = bp.predict(instruction, pc);
      //instruction.params.push_back(prediction);
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
      shelf.dest = n_cpu.alloc(n_cpu, instruction, instruction.params[0], -1);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "movi" )
    {
      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(instruction.params[1], true);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, instruction, instruction.params[0], -1);

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
      shelf.dest = n_cpu.alloc(n_cpu, instruction, instruction.params[0], -1);

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
      shelf.dest = n_cpu.alloc(n_cpu, instruction, instruction.params[0], -1);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);
    }
    else if ( opcode == "b" )
    {
      BRS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(0, true); // prediction
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      shelf.tgt = instruction.params[0];
      shelf.npc = instruction.params[1];
      shelf.pred = instruction.params[2];
      shelf.dest = n_cpu.alloc(n_cpu, instruction, -1, shelf.pred);
 
      n_cpu.rob.set_spec(shelf.dest, false); // unconditional branches are not speculative
      n_cpu.brs.issue(shelf);
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

      BRS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[0]);
      std::tie(shelf.o2, shelf.v2) = n_cpu.read(instruction.params[1]);
      shelf.tgt = instruction.params[2];
      shelf.npc = instruction.params[3];
      shelf.pred = instruction.params[4];
      shelf.dest = n_cpu.alloc(n_cpu, instruction, -1, shelf.pred);
 
      n_cpu.rob.set_spec(shelf.dest, true); // just a formality
      n_cpu.brs.issue(shelf);
    }
    else if ( opcode == "lw" )
    {
      LSQ::Shelf shelf;
      shelf.type = LSQ::Shelf::LOAD;
      std::tie(shelf.w, shelf.vw) = std::make_tuple(0, true);
      std::tie(shelf.b, shelf.vb) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o, shelf.vo) = std::make_tuple(instruction.params[2], true);
      shelf.d = n_cpu.alloc(n_cpu, instruction, instruction.params[0], -1);
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
      shelf.seq = n_cpu.alloc(n_cpu, instruction, -1, -1); // simply the rob entry

      n_cpu.rob.set_spec(shelf.seq, false);
      n_cpu.lsq.issue(shelf, n_cpu.rob.get_tail());
    }
    else if ( opcode == "end" )
    {
      int seq = n_cpu.alloc(n_cpu, instruction, -1, -1);
      n_cpu.rob.set_spec(seq, false);
    }
  }
}

void Processor::execute(Processor &n_cpu)
{
  // determine which ports are available
  std::array<bool,NUM_ALUS> port;
  bool portb;
  bool portm;

  for (std::size_t p = 0; p < NUM_ALUS; ++p)
  {
    port[p] = this->alu[p].duration == 0;
  }
  portb = true;
  portm = this->mu.duration == 0;

  // receive instructions to pass to execution units
  std::array<RS::Shelf,NUM_ALUS> e = this->rs.dispatch(n_cpu.rs, port);
  BRS::Shelf eb = this->brs.dispatch(n_cpu.brs, portb);
  LSQ::Shelf em = this->lsq.dispatch(n_cpu.lsq, portm);

  // dispatch when instruction has finished
  for (std::size_t p = 0; p < NUM_ALUS; ++p)
  {
    if (port[p]) n_cpu.alu[p].dispatch(e[p].opcode, e[p].o1, e[p].o2, e[p].dest);
  }
  if (portb) n_cpu.bu.dispatch(eb);
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
      n_cpu.brs.update(n_cpu.alu[i].dest, n_cpu.alu[i].result);
      n_cpu.lsq.update(n_cpu.alu[i].dest, n_cpu.alu[i].result);
    }
  }

  if (n_cpu.mu.writeback && n_cpu.mu.duration == 0)
  {
    n_cpu.rob.write(n_cpu.mu.shelf.seq, n_cpu.mu.result);
    n_cpu.rs.update(n_cpu.mu.shelf.seq, n_cpu.mu.result);
    n_cpu.brs.update(n_cpu.mu.shelf.seq, n_cpu.mu.result);
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
      n_cpu.brs.update(this->alu[i].dest, this->alu[i].result);
      n_cpu.lsq.update(this->alu[i].dest, this->alu[i].result);
    }
  }

  if (this->bu.writeback)
  {
    n_cpu.rob.write(this->bu.shelf.dest, this->bu.result);
  }

  if (this->mu.writeback && this->mu.duration == 0)
  {
    n_cpu.rob.write(this->mu.shelf.seq, this->mu.result);
    n_cpu.rs.update(this->mu.shelf.seq, this->mu.result);
    n_cpu.brs.update(this->mu.shelf.seq, this->mu.result);
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

#ifdef EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif
    }
    // branch
    else if (entry.type == ROB::Entry::BR)
    {
#ifdef EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif

      if (entry.val != entry.target)
      {
        n_cpu.branch_mispred = this->branch_mispred + 1;
        n_cpu.flush(entry.val);
        break; // following commits refer to mispredicts, so stop
      }
      n_cpu.branch_corpred = this->branch_corpred + 1;
    }
    else if (entry.type == ROB::Entry::SR)
    {
#ifdef EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif
    }
    // end of program
    else if (entry.type == ROB::Entry::END)
    {
      n_cpu.instructions_executed = this->instructions_executed + i;
#ifdef EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif
      return true; // TODO: not counted as an instruction?
    }
  }

  n_cpu.instructions_executed = this->instructions_executed + i;
  //std::cout << "RETIRING: " << i << std::endl;
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

int Processor::alloc(Processor &n_cpu, Instruction instruction, int reg, int target)
{
  int a = n_cpu.rob.push(n_cpu.rob, instruction, reg, target);

  // if valid register
  if (reg != -1)
  {
    this->rat.write(n_cpu.rat, reg, a);
  }

  return a;
}

bool Processor::isStalled()
{
  int rsspace = 0; 
  int brsspace = 0;
  int lsqspace = 0; 
  int robspace = FETCHRATE; 

  int pc; Instruction instruction;
  for (std::size_t i = 0; i < ibuf.size(); ++i)
  {
    std::tie(pc, instruction) = ibuf[i];
    std::string opcode = instruction.opcode;
    if (Instruction::isArth(opcode))
    {
      rsspace++;
    }
    else if (Instruction::isBrch(opcode))
    {
      brsspace++;
    }
    else if (Instruction::isLdsr(opcode))
    {
      lsqspace++;
    }
  }

  return this->rs.space() < rsspace || this->brs.space() < brsspace || this->lsq.space() < lsqspace || this->rob.space() < robspace;
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
#ifdef DEBUG
  std::cout << "=========================" << std::endl;
  std::cout << "=== MISPREDICT BRANCH ===" << std::endl;
  std::cout << "=========================" << std::endl;
#endif

  // flush and jump to entry.target
  for (int i = 0; i < FETCHRATE; ++i)
  {
    this->ibuf[i] = std::make_tuple(0, Instruction());
  }
  this->rat.reset();
  this->rob.reset();
  this->rs.reset();
  this->brs.reset();
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
  this->bht = cpu.bht;
  this->rat = cpu.rat;
  this->rob = cpu.rob;
  this->rrf = cpu.rrf;
  this->rs = cpu.rs;
  this->brs = cpu.brs;
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
  this->instructions_fetched = cpu.instructions_fetched;

#ifdef EXE_TRACE
  this->exe = cpu.exe;
#endif

  return *this;
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
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
  os << "  bht = {\n"
     << "    " << cpu.bht << '\n'
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
  os << "  brs = {\n" << cpu.brs
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
     << "ipc = " << ((double)cpu.instructions_executed / (double)cpu.cycles) << " (best = " << FETCHRATE << ")\n"
     << "bpa = " << (double)cpu.branch_corpred / (double)(cpu.branch_corpred + cpu.branch_mispred) << '\n'
     << "fpc = " << ((double)cpu.instructions_fetched / (double)cpu.cycles) << " (best = " << FETCHRATE << ')';
  return os;
}
