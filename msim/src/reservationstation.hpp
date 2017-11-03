#ifndef RESERVATIONSTATION_H
#define RESERVATIONSTATION_H

#include <cstdint>

class ReservationStation
{
  public:
    struct Entry
    {
      uint32_t OC   = 0; // operation code
      uint32_t R_s1 = 0; // source register 1
      uint32_t R_s2 = 0; // source register 2
      uint32_t R_d  = 0; // destination register
    };

  public:
    Entry entry[32];
};

#endif
