import numpy as np
from math import floor

gap = 1024*64

x = [0]*gap
y = list(range(gap))
z = [1]*gap
X = np.array(x+y+z)
np.savetxt("data",X, "%d")

