  movi r0 0
  movi r1 20
  movi r2 1

:loop
  mul r2 r2 r1
  subi r1 r1 1
  bneq r1 r0 :loop

  end
