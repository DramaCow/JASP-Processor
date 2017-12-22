; init
  movi r0 0
  movi r1 0
  movi r7 8

  movi r2 0
  sw r2 r1 0
  addi r1 r1 1

  movi r3 1
  sw r3 r1 0
  addi r1 r1 1

:loop
  lw r2 r1 -2
  lw r3 r1 -1
  add r4 r2 r3
  sw r4 r1 0
  addi r1 r1 1
  bneq r1 r7 :loop

  end
