#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG
//#define EXE_TRACE

#define FETCHRATE 4
#define RETIRERATE 6

#define NUM_HRT_ENTRIES 64
#define HREG_BITS 4
#define NUM_PT_ENTRIES (1<<HREG_BITS) // used for 2-level
//#define NUM_PT_ENTRIES 64 // used for simple 2bit saturated counter

#define NUM_REGISTERS 8

#define NUM_ROB_ENTRIES 16
#define NUM_RS_ENTRIES 8
#define NUM_BRS_ENTRIES 8
#define NUM_LSQ_ENTRIES 8

#define NUM_ALUS 4

#define BLOCKSIZE 16
#define L1_NUM_SETS 2
#define L1_SET_SIZE 1 // Direct Mapped Cache
#define L2_NUM_SETS 2
#define L2_SET_SIZE 4

//#define L1_ACCESS_COST 2
//#define L2_ACCESS_COST 4
//#define MEM_ACCESS_COST 8
//#define BASE_MU_COST -1

#define L1_ACCESS_COST 0
#define L2_ACCESS_COST 0
#define MEM_ACCESS_COST 0
#define BASE_MU_COST 0

#endif
