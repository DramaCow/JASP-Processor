  movi r4 100

:loop
  addi r0 r0 1
  addi r1 r1 1
  addi r2 r2 1
  addi r3 r3 1
  bneq r0 r4 :loop

  end
