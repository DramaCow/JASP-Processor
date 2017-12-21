#include "processor.hpp"
#include <algorithm>
#include <iomanip>

#define DP3 std::fixed<<std::setprecision(3)
#define HEX8 "0x"<<std::setfill('0')<< std::setw(8)<<std::hex
#define HEX2 "0x"<<std::setfill('0')<< std::setw(2)<<std::hex
#define HEX1 "0x"<<std::setfill('0')<< std::setw(1)<<std::hex
#define DEC std::dec

Processor::Processor(ICache &icache, SAC &l1cache, SAC &l2cache, MEM &mem) :
  icache(icache),
  l1cache(l1cache),
  l2cache(l2cache),
  mem(mem),

  mu(l1cache, l2cache, mem),

  cycles(0),
  branch_corpred(0),
  branch_mispred(0),
  instructions_fetched(0),
  instructions_issued(0),
  instructions_dispatched(0),
  instructions_executed(0)
{
}

// ==========================
// === PIPELINE FUNCTIONS ===
// ==========================

bool Processor::tick(Processor &n_cpu)
{
  n_cpu.cycles++;

  execute(n_cpu);
  writeback(n_cpu);
  decode(n_cpu);
  fetch(n_cpu);
  return commit(n_cpu); // done last in-case of branch mispredict
}

void Processor::fetch(Processor &n_cpu)
{
  int pc = this->pc;

  int i;
  for (i = 0; i < FETCHRATE && n_cpu.ibuf.size() < IBUF_MAX_SIZE; ++i)
  {
    Instruction instruction = icache[pc];
    int npc = pc + 1;

    // pre-decode (branch detect)
    if (Instruction::isBrch(instruction.opcode))
    {
      //bool pred = true;
      //npc = instruction.getTakenBTA();

      //bool pred = this->pt.predict(pc);
      //npc = pred ? instruction.getTakenBTA() : pc+1;
      //instruction.npc = pc+1;
      //instruction.pred = pred;

      int pattern = this->hrt.history(pc);
      bool pred = this->pt.predict(pattern);
      npc = pred ? instruction.getTakenBTA() : pc+1;
      instruction.npc = pc+1;
      instruction.pattern = pattern;
      instruction.pred = pred;
    }

    n_cpu.ibuf.push_back(std::make_tuple(pc, instruction));
    pc = npc;
  }

  n_cpu.pc = pc;

  n_cpu.instructions_fetched = this->instructions_fetched + i;
}

void Processor::decode(Processor &n_cpu)
{
  int instructionsIssued = 0;

  while (instructionsIssued < ISSUERATE && n_cpu.ibuf.size() > 0)
  {
    int pc; Instruction instruction;
    std::tie(pc, instruction) = n_cpu.ibuf[0];
    std::string opcode = instruction.opcode;

    if ( opcode == "mov" )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.rs.space() == 0)
      {
        break;
      }

      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, pc, instruction, instruction.params[0]);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "movi" )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.rs.space() == 0)
      {
        break;
      }

      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(instruction.params[1], true);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      shelf.dest = n_cpu.alloc(n_cpu, pc, instruction, instruction.params[0]);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "add" ||
              opcode == "sub" ||
              opcode == "mul" ||
              opcode == "xor"    )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.rs.space() == 0)
      {
        break;
      }

      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o2, shelf.v2) = n_cpu.read(instruction.params[2]);
      shelf.dest = n_cpu.alloc(n_cpu, pc, instruction, instruction.params[0]);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "addi" ||
              opcode == "subi" ||
              opcode == "muli"    )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.rs.space() == 0)
      {
        break;
      }

      RS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(instruction.params[2], true);
      shelf.dest = n_cpu.alloc(n_cpu, pc, instruction, instruction.params[0]);

      n_cpu.rob.set_spec(shelf.dest, false);
      n_cpu.rs.issue(shelf);

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "b" )
    {
/*
      BRS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = std::make_tuple(0, true); // prediction
      std::tie(shelf.o2, shelf.v2) = std::make_tuple(0, true); // not used
      shelf.tgt = instruction.params[0];
      shelf.npc = instruction.npc;
      shelf.pred = instruction.pred;
      shelf.dest = n_cpu.alloc(n_cpu, pc, instruction, -1, shelf.pred, instruction.pattern);
 
      n_cpu.rob.set_spec(shelf.dest, false); // unconditional branches are not speculative
      n_cpu.brs.issue(shelf);
*/
      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "beq"  ||
              opcode == "bneq" ||
              opcode == "blt"  ||
              opcode == "ble"  ||
              opcode == "bgt"  ||
              opcode == "bge"     )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.brs.space() == 0)
      {
        break;
      }

      int target = instruction.params[2];
      bool prediction = instruction.params[3];

      BRS::Shelf shelf;
      shelf.opcode = opcode;
      std::tie(shelf.o1, shelf.v1) = n_cpu.read(instruction.params[0]);
      std::tie(shelf.o2, shelf.v2) = n_cpu.read(instruction.params[1]);
      shelf.tgt = instruction.params[2];
      shelf.npc = instruction.npc;
      shelf.pred = instruction.pred;
      shelf.dest = n_cpu.alloc(n_cpu, pc, instruction, -1, shelf.pred, instruction.pattern);
 
      n_cpu.rob.set_spec(shelf.dest, true); // just a formality
      n_cpu.brs.issue(shelf);

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "lw" )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.lsq.space() == 0)
      {
        break;
      }

      LSQ::Shelf shelf;
      shelf.type = LSQ::Shelf::LOAD;
      std::tie(shelf.w, shelf.vw) = std::make_tuple(0, true);
      std::tie(shelf.b, shelf.vb) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o, shelf.vo) = std::make_tuple(instruction.params[2], true);
      shelf.d = n_cpu.alloc(n_cpu, pc, instruction, instruction.params[0]);
      shelf.seq = shelf.d;

      n_cpu.rob.set_spec(shelf.seq, false);
      n_cpu.lsq.issue(shelf, n_cpu.rob.get_tail());

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "sw" )
    {
      if (n_cpu.rob.space() == 0 || n_cpu.lsq.space() == 0)
      {
        break;
      }

      LSQ::Shelf shelf;
      shelf.type = LSQ::Shelf::STORE;
      std::tie(shelf.w, shelf.vw) = n_cpu.read(instruction.params[0]);
      std::tie(shelf.b, shelf.vb) = n_cpu.read(instruction.params[1]);
      std::tie(shelf.o, shelf.vo) = std::make_tuple(instruction.params[2], true);
      shelf.seq = n_cpu.alloc(n_cpu, pc, instruction, -1); // simply the rob entry

      n_cpu.rob.set_spec(shelf.seq, false);
      n_cpu.lsq.issue(shelf, n_cpu.rob.get_tail());

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    else if ( opcode == "end" )
    {
      if (n_cpu.rob.space() == 0)
      {
        break;
      }

      int seq = n_cpu.alloc(n_cpu, pc, instruction, -1);
      n_cpu.rob.set_spec(seq, false);

      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
    //else if ( opcode == "nop" )
    else
    {
      instructionsIssued++;
      n_cpu.ibuf.erase(std::begin(n_cpu.ibuf));
    }
  }

  n_cpu.instructions_issued = this->instructions_issued + instructionsIssued;
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

  // === statistics === 

  int instructionsDispatched = 0;
  for (int i = 0; i < NUM_ALUS; ++i)
  {
    instructionsDispatched += port[i];
  }
  instructionsDispatched += portb;
  instructionsDispatched += portm;

  n_cpu.instructions_dispatched = this->instructions_dispatched + instructionsDispatched;
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
    n_cpu.rob.write(this->bu.shelf.dest, this->bu.result, this->bu.taken);
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

#if EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif
    }
    // branch
    else if (entry.type == ROB::Entry::BR)
    {
#if EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif

      n_cpu.hrt.update(entry.pc, entry.taken);
      n_cpu.pt.update(entry.pattern, entry.taken);
      //n_cpu.pt.update(entry.pc, entry.taken);

      // if mispredicted
      if (entry.pred != entry.taken)
      {
        n_cpu.branch_mispred = this->branch_mispred + 1;
        n_cpu.flush(entry.val);
        break; // subsequent commits are incorrectly speculated, so stop
      }
      n_cpu.branch_corpred = this->branch_corpred + 1;
    }
    else if (entry.type == ROB::Entry::SR)
    {
#if EXE_TRACE
      n_cpu.exe.push_back(entry.instruction); // debug
#endif
    }
    // end of program
    else if (entry.type == ROB::Entry::END)
    {
      n_cpu.instructions_executed = this->instructions_executed + i;
#if EXE_TRACE
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

int Processor::alloc(Processor &n_cpu, int pc, Instruction instruction, int reg)
{
  return this->alloc(n_cpu, pc, instruction, reg, false, 0);
}

int Processor::alloc(Processor &n_cpu, int pc, Instruction instruction, int reg, bool pred, int pattern)
{
  int a = n_cpu.rob.push(n_cpu.rob, pc, instruction, reg, pred, pattern);

  // if valid register
  if (reg != -1)
  {
    this->rat.write(n_cpu.rat, reg, a);
  }

  return a;
}

void Processor::flush(int target)
{
#if DEBUG
  std::cout << "=========================" << std::endl;
  std::cout << "=== MISPREDICT BRANCH ===" << std::endl;
  std::cout << "=========================" << std::endl;
#endif

  // flush and jump to entry.target
  //for (int i = 0; i < FETCHRATE; ++i)
  //{
  //  this->ibuf[i] = std::make_tuple(0, Instruction());
  //}
  this->ibuf.clear();
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
  this->hrt = cpu.hrt;
  this->pt = cpu.pt;
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
  this->branch_corpred = cpu.branch_corpred;
  this->branch_mispred = cpu.branch_mispred;
  this->instructions_fetched = cpu.instructions_fetched;
  this->instructions_issued = cpu.instructions_issued;
  this->instructions_dispatched = cpu.instructions_dispatched;
  this->instructions_executed = cpu.instructions_executed;

#if EXE_TRACE
  this->exe = cpu.exe;
#endif

  return *this;
}

std::ostream& operator<<(std::ostream& os, const Processor& cpu)
{
  os << "{\n";
  os << "  pc = " << cpu.pc << '\n';
  os << "  ibuf(" << cpu.ibuf.size() << ") = {\n";
  for (std::size_t i = 0; i < cpu.ibuf.size(); ++i)
  {
    int pc; Instruction instruction;
    std::tie(pc, instruction) = cpu.ibuf[i];
    os << "    " << pc << ": " << instruction << '\n';
  }
  os << "  }\n";
  os << "  hrt = {\n"
     << "    " << cpu.hrt << '\n'
     << "  }\n";
  os << "  pt = {\n"
     << "    " << cpu.pt << '\n'
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
  os << "}\n";
  os << "=== L1 CACHE ===\n"
     << cpu.l1cache;
  os << "=== L2 CACHE ===\n"
     << cpu.l2cache;
  os << "=== statistics ===\n"
     << "cycles = " << cpu.cycles << '\n'
     << "branch-pred-acc = " << DP3 << ((double)cpu.branch_corpred / (double)(cpu.branch_corpred + cpu.branch_mispred))
                 << " = " << cpu.branch_corpred << '/' << cpu.branch_corpred + cpu.branch_mispred << '\n'
     << "fetches-per-cycle    = " << DP3 << ((double)cpu.instructions_fetched / (double)cpu.cycles) << " (best = " << FETCHRATE << ")\n"
     << "issues-per-cycle     = " << DP3 << ((double)cpu.instructions_issued / (double)cpu.cycles) << " (best = " << ISSUERATE << ")\n"
     << "dispatches-per-cycle = " << DP3 << ((double)cpu.instructions_dispatched / (double)cpu.cycles) << " (best = " << DISPATCHRATE << ")\n"
     << "commits-per-cycle    = " << DP3 <<((double)cpu.instructions_executed / (double)cpu.cycles) << " (best = " << RETIRERATE << ")\n";
  os << std::resetiosflags;
  return os;
}
