import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

#mpl.rcParams['agg.path.chunksize'] = 10000

y0 = np.loadtxt("cycles_0")
y1 = np.loadtxt("cycles_1")
y2 = np.loadtxt("cycles_2")

x = np.arange(2,2*len(y0)+2,2)

#plt.ylim(ymin=0, ymax=40000)
plt.xlabel("stride length k")
plt.ylabel("cycles")
plt.title("Cycles given variable memory stride length")
l1, = plt.plot(x, y0, markersize=2)
#l2, = plt.plot(x, y1, markersize=2)
l3, = plt.plot(x, y2, markersize=2)
#plt.legend([l1,l2,l3],["no cache", "L1 cache only", "L1 and L2 cache"],loc="lower right")
plt.legend([l1,l3],["no cache", "L1 and L2 cache"],loc="lower right")
plt.savefig("data.png")
