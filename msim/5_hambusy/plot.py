import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

#mpl.rcParams['agg.path.chunksize'] = 10000

reps = 30

arr = np.loadtxt("bpa_0") ; y0 = np.mean(arr.reshape(-1, reps), axis=1)
arr = np.loadtxt("bpa_1") ; y1 = np.mean(arr.reshape(-1, reps), axis=1)
arr = np.loadtxt("bpa_2") ; y2 = np.mean(arr.reshape(-1, reps), axis=1)
arr = np.loadtxt("bpa_3") ; y3 = np.mean(arr.reshape(-1, reps), axis=1)
arr = np.loadtxt("bpa_4") ; y4 = np.mean(arr.reshape(-1, reps), axis=1)

x = np.arange(1,len(y1)+1,1)

plt.ylim([0,1])
plt.plot(x, y0, markersize=2)
plt.plot(x, y1, markersize=2)
plt.plot(x, y2, markersize=2)
plt.plot(x, y3, markersize=2)
plt.plot(x, y4, markersize=2)
plt.savefig("data.png")
