  movi r0 0
  movi r1 12

:loop
  movi r2 0
  addi r0 r0 1
  movi r2 0
  addi r0 r0 1
  bneq r0 r1 :loop

  end  
