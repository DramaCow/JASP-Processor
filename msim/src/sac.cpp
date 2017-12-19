#include "sac.hpp"

SAC::SAC(int n, int k) :
  lines(n*k), numSets(n), setSize(k)
{
}

std::tuple<int,bool> SAC::load(int addr)
{
  int off = addr % BLOCKSIZE;             // offset in block/line
  int idx = (addr / BLOCKSIZE) % numSets; // which set of lines
  int tag = (addr / BLOCKSIZE) / numSets; // global mem-id on block

  int startLine = setSize*idx; // meta-index of first line in set

  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (this->lines[l].tag == tag)
    {
      // cache-hit
      this->adjustLRU(idx, l);
      return std::make_tuple(this->lines[l].data[off], true);
    }
  }

  // cache-miss
  return std::make_tuple(0, false);
}

bool SAC::store(int addr, int val)
{
  int off = addr % BLOCKSIZE;                   // offset in block/line
  int idx = (addr / BLOCKSIZE) % this->numSets; // which set of lines
  int tag = (addr / BLOCKSIZE) / this->numSets; // global mem-id on block

  int startLine = this->setSize*idx; // meta-index of first line in set

  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (this->lines[l].tag == tag)
    {
      // cache-hit
      this->adjustLRU(idx, l);
      this->lines[l].data[off] = val;
      this->lines[l].dirty = true;
      return true;
    }
  }

  // cache-miss
  return false;
}

std::tuple<SAC::Line,bool> SAC::getline(int a)
{
  int idx = a % this->numSets; // which set of lines
  int tag = a / this->numSets; // global mem-id on block

  int startLine = this->setSize*idx; // meta-index of first line in set

  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (this->lines[l].tag == tag)
    {
      // line exists in cache
      this->adjustLRU(idx, l);
      return std::make_tuple(this->lines[l], true);
    }
  }

  // line doesn't exist in cache (TODO: better dummy value?)
  return std::make_tuple(Line(), false);
} 

void SAC::writeline(int a, Line line)
{
  int idx = a % this->numSets; // which set of lines
  int tag = a / this->numSets; // global mem-id on block

  int startLine = this->setSize*idx; // meta-index of first line in set

  // first pass; check if any lines are invalid --> can be used
  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (!this->lines[l].valid)
    {
      this->lines[l] = line;
      this->lines[l].lru = 0;
      this->adjustLRU(idx, l);
      return;
    }
  }

  // second pass; pick Least Recently Used line to replace
  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (!this->lines[l].lru == 0)
    {
      this->lines[l] = line;
      this->lines[l].lru = 0;
      this->adjustLRU(idx, l);
      return;
    }
  }
}

void SAC::adjustLRU(int idx, int l)
{
  int startLine = this->setSize*idx; // meta-index of first line in set

  // count number of valid lines in set AND
  // decrement all counters above original counter of lines[l]
  int validCount = 0;
  for (int k = startLine; k < startLine + this->setSize; ++k)
  {
    if (this->lines[k].valid)
    {
      // count
      validCount++;

      // decrement
      if (k != l && this->lines[k].lru > this->lines[l].lru)
      {
        this->lines[k].lru--;
      }
    }
  }

  // set lru value
  this->lines[l].lru = validCount-1;
}
