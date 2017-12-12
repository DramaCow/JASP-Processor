import numpy as np

x = np.arange(0,64,1,dtype=int)
np.savetxt("data",x, "%d")
