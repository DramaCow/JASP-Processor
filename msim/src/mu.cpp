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
  return os;
}
