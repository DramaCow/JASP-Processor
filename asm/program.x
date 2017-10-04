  xor r0 r0 r0
  addi r2 r0 10
:loop
  bnez r2 :break
  addi r1 r1 1
  subi r2 r1 10
  j :loop
:break
