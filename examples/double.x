  movi r0 0
  movi r1 0
  movi r7 64

:loop
  lw r2 r1 0
  muli r2 r2 2
  sw r2 r1 0
  addi r1 r1 1
  bneq r1 r7 :loop

  end
