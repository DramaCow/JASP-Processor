 ; zero
  movi r0 0
 ; index
  movi r1 1
 ; loop limit
  lw r2 r0 0
  addi r2 r2 1
 ; hamming weight
  movi r3 0

:loop
  lw r4 r1 0
  beq r4 r0 :skip
  addi r3 r3 1
:skip
  addi r1 r1 1
  bneq r1 r2 :loop

  end
