  addi r0 r0 100
:loop
  addi r1 r1 1
  addi r2 r2 1
  addi r3 r3 1
  addi r4 r4 1
  bneq r0 r1 :loop
  end
