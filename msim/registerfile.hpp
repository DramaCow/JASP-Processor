#ifndef REGISTERFILE_H
#define REGISTERFILE_H

#include <cstdint>
#include <tuple>
#include <iostream>

class RegisterFile
{
  public:
    RegisterFile();
    std::tuple<uint32_t, uint32_t> foo(uint32_t addr1, uint32_t addr2, uint32_t daddr, uint32_t data, bool write);
    friend std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile);

  private:
    uint32_t gpr[32] = { 0 };
};

std::ostream& operator<<(std::ostream& os, const RegisterFile& regfile);

#endif
