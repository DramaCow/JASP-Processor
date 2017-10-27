#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include <cstdint>
#include <tuple>
#include <iostream>

#define NUM_REGISTERS 4

class RegisterFile
{
  public:
    RegisterFile();
    std::tuple<uint32_t, uint32_t> foo(uint32_t addr1, uint32_t addr2, uint32_t daddr, uint32_t data, bool we);
    friend std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile);
    RegisterFile& operator=(const RegisterFile& regfile);

  private:
    uint32_t gpr[NUM_REGISTERS] = { 0 };
    bool scoreboard[NUM_REGISTERS];
};

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile);

#endif
