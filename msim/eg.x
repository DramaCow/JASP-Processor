  ldc r0 0
  ldc r1 10
:loop
  subi r1 r1 1
  beq r0 r1 :end
  b :loop
:end
