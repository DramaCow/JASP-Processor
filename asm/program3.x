  xor r0 r0 r0
  addi r0 r0 10
:l1
  subi r0 r0 1
  jnez r0 :l1
:l2
  addi r0 r0 1
  j :l2
