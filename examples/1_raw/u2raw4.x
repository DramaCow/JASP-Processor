  movi r0 0
  movi r1 2520

:loop
  movi r2 0
  add r3 r3 r2
  add r4 r4 r3
  add r5 r5 r4
  addi r0 r0 1
  movi r2 0
  add r3 r3 r2
  add r4 r4 r3
  add r5 r5 r4
  addi r0 r0 1
  movi r2 0
  add r3 r3 r2
  add r4 r4 r3
  add r5 r5 r4
  addi r0 r0 1
  bneq r0 r1 :loop

  end  
