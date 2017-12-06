  ; x vector
  movi r0 1

  ; y vector
  movi r1 5

  ; idx and limit
  movi r6 1
  movi r7 5

  ; dot product
  movi r2 0

:loop
  mul r3 r0 r1
  add r2 r2 r3

  addi r0 r0 1
  addi r1 r1 1

  addi r6 r6 1
  bneq r6 r7 :loop

  end
