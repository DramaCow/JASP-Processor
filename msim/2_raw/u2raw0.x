  movi r0 0
  movi r1 2520

:loop
  addi r0 r0 1
  addi r0 r0 1
  addi r0 r0 1
  bneq r0 r1 :loop

  end  
