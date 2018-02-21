  movi r7 1000

:loop
  addi r0 r0 1
  addi r1 r1 1
  addi r2 r2 1
  addi r3 r3 1
  addi r4 r4 1
  addi r5 r5 1
  addi r6 r6 1
  bneq r0 r7 :loop

  end
