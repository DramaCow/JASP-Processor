#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>
#include <iostream>

class Memory
{
  public:
    Memory(int capacity);
    uint32_t operator[](int a);
    friend std::ostream& operator<<(std::ostream& os, const Memory& mem);
    void copy(int offset, uint8_t *bytes, int size);

    const int capacity;
    uint8_t *bytes;
};

std::ostream& operator<<(std::ostream& os, const Memory& mem);

#endif
