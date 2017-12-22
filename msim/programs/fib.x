  movi r0 0
  movi r1 1

  movi r6 0
  movi r7 30

:loop
  add r2 r0 r1
  mov r0 r1
  mov r1 r2
  addi r6 r6 1
  bneq r6 r7 :loop

  end
