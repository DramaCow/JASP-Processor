import numpy as np

x = np.arange(0,512,1,dtype=int)
np.random.shuffle(x)
np.savetxt("data",x, "%d")
