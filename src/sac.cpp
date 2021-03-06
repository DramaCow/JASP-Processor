#include "sac.hpp"

SAC::SAC(int n, int k) :
  lines(n*k), numSets(n), setSize(k)
{
}

SAC::Line::Line() :
  valid(false)
{
  std::fill(std::begin(this->data), std::end(this->data), 0);
}

SAC::Line::Line(std::array<int,BLOCKSIZE> data, int baddr) :
  data(data),
  valid(true),
  baddr(baddr),
  dirty(false),
  lru(0)
{
}

SAC::Line::Line(const SAC::Line &line)
{
  (*this) = line;
}

SAC::Line * SAC::access(int baddr)
{
  int idx = baddr % numSets; // which set of lines
  int tag = baddr / numSets; // global mem-id on block

  int startLine = setSize*idx; // meta-index of first line in set

  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (this->lines[l].valid && this->lines[l].tag == tag)
    {
      // cache-hit
      this->adjustLRU(idx, l);
      return &this->lines[l];
    }
  }

  // cache-miss
  return nullptr;
}

std::tuple<SAC::Line*,SAC::Line*> SAC::stash(int baddr, Line *line)
{
  int idx = baddr % this->numSets; // which set of lines
  int tag = baddr / this->numSets; // global mem-id on block

  int startLine = this->setSize*idx; // meta-index of first line in set
  SAC::Line *replaceLine = nullptr;  // copy of line being replaced

  // first pass; check if line exists in cache, override if so
  // (strictly necessary for L1-replaceLine --> L2)
  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (this->lines[l].valid && this->lines[l].tag == tag)
    {
      this->lines[l] = (*line);
      this->lines[l].tag = tag;
      this->lines[l].lru = 0;
      this->adjustLRU(idx, l);
      return std::make_tuple(&this->lines[l],replaceLine);
    }
  }

  // second pass; check if any lines are invalid --> can be used
  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (!this->lines[l].valid)
    {
      this->lines[l] = (*line);
      this->lines[l].tag = tag;
      this->lines[l].lru = 0;
      this->adjustLRU(idx, l);
      return std::make_tuple(&this->lines[l],replaceLine);
    }
  }

  // third pass; pick Least Recently Used line to replace
  for (int l = startLine; l < startLine + this->setSize; ++l)
  {
    if (this->lines[l].lru == 0)
    {
      // copy of line being replaced (only if dirty for writeback)
      if (this->lines[l].dirty)
      {
        replaceLine = new SAC::Line(this->lines[l]);
      }

      this->lines[l] = (*line);
      this->lines[l].tag = tag;
      this->lines[l].lru = 0;
      this->adjustLRU(idx, l);
      return std::make_tuple(&this->lines[l],replaceLine);
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

std::ostream& operator<<(std::ostream& os, const SAC& sac)
{
  for (int j = 0; j < sac.numSets; ++j)
  {
    for (int i = 0; i < sac.setSize; ++i)
    {
      //SAC::Line& line = sac.lines[sac.setSize*j + i];
      SAC::Line line = sac.lines[sac.setSize*j + i];
      if (line.valid)
      {
        os << "baddr: " << line.baddr << " tag: " << line.tag << " dirty: " << line.dirty << " lru: " << line.lru << '\n';
        for (int k = 0; k < BLOCKSIZE; ++k)
        {
          os << line.data[k] << ' ';
        }
        os << '\n';
      }
      else
      {
        os << "invalid\n-------\n";
      }
    }

    if (j < sac.numSets-1)
    {
      os << "~~~\n";
    }
  }

  return os;
}
