  movi r0 0
  movi r1 0
  movi r2 0
  movi r3 7

:outer
:inner
  lw r4 r2 0
  lw r5 r2 1
  ble r4 r5 :skip
  sw r5 r2 0
  sw r4 r2 1
:skip
  addi r2 r2 1
  bneq r2 r3 :inner
  movi r2 0
  addi r1 r1 1
  bneq r1 r3 :outer

  end
