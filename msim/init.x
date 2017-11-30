  ; x vector
  movi r0 1

  ; y vector
  movi r1 5

  ; idx and limit
  movi r14 0
  movi r15 1

:loop
  addi r0 r0 1
  addi r1 r1 1

  addi r14 r14 1
  bneq r14 r15 :loop

  end
