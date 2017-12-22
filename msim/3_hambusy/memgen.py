import numpy as np
from math import floor
import sys

if len(sys.argv) <= 1:
  print("usage: python plot.py <input_filename>")
else:
  length = 1024
  k = int(sys.argv[1])
  r = length - floor(length/k)*k
  if k == 1:
    pattern = np.random.randint(0, 2)
  else: 
    pattern = np.random.randint(1, 2**k - 1)
  x = [int(b) for b in np.binary_repr(pattern,width=k)]
  #print(x)
  X = x*floor(length/k) + x[0:r]
  #print(sum(X))
  X = np.array([length] + X)
  np.savetxt("data", X, "%d")
