#ifndef LSQ_H
#define LSQ_H

#include <string>
#include <vector>
#include <iostream>
#include "config.hpp"
#include "cache.hpp"

// Load-Store Uueue
class LSQ
{
  public:
    struct Shelf
    {
      enum Type { NA, LOAD, STORE };
      Type type = NA;

      int seq = 0; // effective sequence id

      int d = 0; // dest (used by load)
      int w = 0; bool vw = true; // write data (used by store)

      int b = 0; bool vb = true; // base
      int o = 0; bool vo = true; // offset
      int addr = 0; bool va = false; // address
      
      bool ready = false;

      bool fwd = false; // was val forwarded?
    };

  public:
    int space() const;

    void issue(Shelf shelf, int tail);
    Shelf dispatch(LSQ &n_lsq, bool port); // note: dispatch does not remove any entries

    void retire(int seq);
    void mark(int seq);
    void update(int dest, int result);
    void reset();

    LSQ& operator=(const LSQ& lsq);

    friend std::ostream& operator<<(std::ostream& os, const LSQ& lsq);

  private:
    std::vector<Shelf> shelves;
    std::vector<bool> isNew; // meta

    int get_shelf(int seq);
    void store_to_load_forward(int i);
};
std::ostream& operator<<(std::ostream& os, const LSQ& lsq);

#endif
