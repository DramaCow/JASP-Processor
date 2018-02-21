  movi r0 0

  movi r1 0
  movi r2 1024
  movi r3 0
  movi r4 32

:loop
  lw r5 r3 0
  sw r5 r1 0
  addi r1 r1 1
  add r3 r3 r4
  bneq r1 r2 :loop

  end
