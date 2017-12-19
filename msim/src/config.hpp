#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG
//#define EXE_TRACE

#define FETCHRATE 4
#define RETIRERATE 6

#define NUM_BHT_ENTRIES 64
#define NUM_REGISTERS 8
#define NUM_ROB_ENTRIES 16
#define NUM_RS_ENTRIES 8
#define NUM_BRS_ENTRIES 8
#define NUM_LSQ_ENTRIES 8
#define NUM_ALUS 4

#define BLOCKSIZE 16
#define MEM_NUM_BLOCKS 16
#define L1_NUM_LINES 2
#define L1_SET_SIZE 1
#define L2_NUM_LINES 16
#define L2_SET_SIZE 16

#endif
