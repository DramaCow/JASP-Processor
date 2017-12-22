import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

#mpl.rcParams['agg.path.chunksize'] = 10000
 
y1 = np.loadtxt("epc_raw")
y2 = np.loadtxt("epc_uraw")
y3 = np.loadtxt("epc_u2raw")
y4 = np.loadtxt("epc_curaw")

z1 = np.loadtxt("epc_short_raw")
z2 = np.loadtxt("epc_short_uraw")
z3 = np.loadtxt("epc_short_u2raw")
z4 = np.loadtxt("epc_short_curaw")

x = np.arange(0,len(y1),1)

plt.title("Throughput of program 1 under infinite resource (n=2520)")
plt.xlabel("length of per-iteration dependent instruction sequence k")
plt.ylabel("throughput (commits-per-cycle)")
plt.ylim([0,8])
l1, = plt.plot(x, y1, markersize=2)
l2, = plt.plot(x, y2, markersize=2)
l3, = plt.plot(x, y3, markersize=2)
l4, = plt.plot(x, y4, markersize=2)
plt.legend([l1,l2,l3,l4],["No unroll", "Once unrolled", "Twice unrolled", "Completely unrolled"],loc="upper left")

plt.savefig("raw1.png")

plt.clf()

plt.title("Throughput of program 1 under infinite resource (n=12)")
plt.xlabel("length of per-iteration dependent instruction sequence k")
plt.ylabel("throughput (commits-per-cycle)")
plt.ylim([0,8])
l1, = plt.plot(x, z1, markersize=2)
l2, = plt.plot(x, z2, markersize=2)
l3, = plt.plot(x, z3, markersize=2)
l4, = plt.plot(x, z4, markersize=2)
plt.legend([l1,l2,l3,l4],["No unroll", "Once unrolled", "Twice unrolled", "Completely unrolled"],loc="upper left")

plt.savefig("raw2.png")
