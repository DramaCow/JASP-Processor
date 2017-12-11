#include "mu.hpp"
#include <iomanip>

#define SPACE std::left<<std::setfill((char)32)<<std::setw(6)<<

MU::MU(DCache &dcache) :
  dcache(dcache)
{
}

bool MU::isReady()
{
  return true;
}

void MU::dispatch(LSQ::Shelf shelf)
{
  std::cout << "MUHERE\n";
  std::cout << SPACE(shelf.type == LSQ::Shelf::LOAD ? "L" : shelf.type == LSQ::Shelf::STORE ? "S" : "--")
     << SPACE(std::to_string(shelf.seq))
     << SPACE(std::to_string(shelf.d))
     << SPACE((shelf.vw ? std::string("") : std::string("d")) + std::to_string(shelf.w))
     << SPACE((shelf.vb ? std::string("") : std::string("d")) + std::to_string(shelf.b))
     << SPACE((shelf.vo ? std::string("") : std::string("d")) + std::to_string(shelf.o))
     << SPACE((shelf.va ? std::string("") : std::string("d")) + std::to_string(shelf.addr))
     << SPACE(std::to_string(shelf.ready))
     << '\n';
  std::cout << (*this) << std::endl;

  this->shelf = shelf;

  if (this->shelf.type == LSQ::Shelf::LOAD)
  {
    this->result = dcache[this->shelf.addr];
    this->duration = 1;
    this->writeback = true;
  }
  else if (this->shelf.type == LSQ::Shelf::STORE)
  {
    this->duration = 1;
    this->writeback = true;
  }
  else
  {
    this->duration = 0;
    this->writeback = false;
  }

  std::cout << (*this) << std::endl;
}

// applied to the current state, updates the next state
void MU::execute(MU& n_mu)
{
  n_mu.duration = this->duration-1 > 0 ? this->duration-1 : 0;
}

MU& MU::operator=(const MU& mu)
{
  this->shelf = mu.shelf;
  this->result = mu.result;
  this->duration = mu.duration;
  this->writeback = mu.writeback;
  return *this;
}

std::ostream& operator<<(std::ostream& os, const MU& mu)
{
  if (mu.shelf.type == LSQ::Shelf::NA)
  {
    os << "    --- nop ---\n";
    return os;
  }
  else if (mu.shelf.type == LSQ::Shelf::LOAD)
  {
    os << "    d" << mu.shelf.d << " <-- MEM[" << mu.shelf.addr << ']';
  }
  else if (mu.shelf.type == LSQ::Shelf::STORE)
  {
    os << "    MEM[" << mu.shelf.addr << "] <-- " << mu.shelf.w;
  }

  os << " (" << mu.duration << ")\n";
  if (mu.writeback && mu.duration == 0)
  {
    os << "    ";
    if (mu.shelf.type == LSQ::Shelf::LOAD)
    {
      os << "= " << mu.result << ' ';
    }
    os << "writeback\n";
  }
  
  return os;
}
