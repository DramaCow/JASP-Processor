  movi r0 0
  movi r1 1000
:loop
  addi r0 r0 1
  beq r0 r1 :end
  b :loop
:end
  end
