import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

#mpl.rcParams['agg.path.chunksize'] = 10000

arr = np.loadtxt("bpa")
y1 = np.mean(arr.reshape(-1, 10), axis=1)

x = np.arange(1,len(y1)+1,1)

plt.ylim([0,1])
plt.plot(x, y1, markersize=2)
plt.savefig("data.png")
