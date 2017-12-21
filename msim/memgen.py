import numpy as np
from math import floor

'''
X = np.arange(0,64,1,dtype=int)
np.random.shuffle(X)
'''

length = 1024
k = 3
r = length - floor(length/k)*k
pattern = np.random.randint(1, 2**k - 1)
x = [int(b) for b in np.binary_repr(pattern,width=k)]
X = x*floor(length/k) + x[0:r]
X = np.array(X)
print(sum(X))

np.savetxt("data",X, "%d")

