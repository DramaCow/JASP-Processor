folders:
  asm  - assembler
  data - data cache hex file generator
  msim - processor simulator

each program is compiled by:
  $ make

to run the simulator:
  $ ./msim a.hex

example program:
  (a.hex contains code for incrementing to r1-r4 repeatedly. This is an example program that doesn't have
   any data dependencies)

interpreting the output:
  For each tick (t = time), values in the register file and at each latch section thing of the pipeline are displayed.
  Register file outputs dirty bit after underscore.
  Number of ticks is set at the top of msim/src/main.cpp (LIMIT), and is pretty small at the moment.
  Values of instruction cache and data cache are outputted at the end of execution, and is just a big array of integers.
  I realise the latch stuff is probably too detailed.
  Opcodes are described at the top of asm/main.cpp

processor features:
  Currently is a very simple pipelined processor that updates all the latches simultaneously. Doesn't have reallocation station or renaming or reorder buffer. 
  There are 5 stages to the pipeline:
   1. (f)etch
   2. (d)ecode
   3. (e)xecute
   4. (m)emory access
   5. (w)riteback
  With latches between each adjacent pairs of states e.g. lat_f_d is the latch struct between the fetch and decode state.
