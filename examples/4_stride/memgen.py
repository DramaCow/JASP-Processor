import numpy as np
from math import floor

X = np.arange(0,1024*256,1,dtype=int)
np.savetxt("data",X, "%d")

