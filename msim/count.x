  addi r1 r1 100
:loop
  addi r0 r0 1
  bneq r0 r1 :loop
  end
