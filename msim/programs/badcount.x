  addi r1 r1 100
:loop
  addi r0 r0 1
  beq r0 r1 :end
  b :loop
:end
  end