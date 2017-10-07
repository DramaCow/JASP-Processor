  xor r0 r0 r0
  xor r1 r1 r1

:l1
  sti r1 r1 0
  addi r1 r1 1
  subi r2 r1 10
  jnez r2 :l1

  xor r1 r1 r1
  xor r2 r2 r2

:l2
  ldi r3 r1 0
  add r2 r2 r3
  addi r1 r1 1
  subi r3 r1 10
  jnez r3 :l2
