  movi r0 0

  movi r1 0
  movi r2 1024
  movi r3 0
  movi r4 1

:loop
  lw r5 r3 65536
  lw r6 r3 131072
  add r7 r5 r6
  sw r7 r3 0
  addi r1 r1 1
  add r3 r3 r4
  bneq r1 r2 :loop

  end
