:main
  movi r0 0
  movi r1 3
  movi r2 0
  movi r3 120

:loop
  addi r0 r0 1
  addi r2 r2 1
  beq r0 r1 :third
  b :loop

:third
  movi r0 0
  bneq r2 r3 :loop

:finish
  end
