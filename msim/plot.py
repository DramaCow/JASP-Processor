import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

#mpl.rcParams['agg.path.chunksize'] = 10000

y0 = [0.754, 0.907, 0.929, 0.929] # bubble sort
y1 = [0.959, 1.821, 2.250, 2.250] # fib
y2 = [0.899, 0.899, 0.912, 0.912] # factorial
y3 = [0.980, 1.911, 2.791, 3.626] # dot product

x = [1,2,3,4]

#plt.ylim(ymin=0, ymax=40000)
plt.xlabel("pipeline width")
plt.ylabel("instructions-per-cycle")
plt.title("Scaling performance of select programs")
l0, = plt.plot(x, y0, markersize=2)
l1, = plt.plot(x, y1, markersize=2)
l2, = plt.plot(x, y2, markersize=2)
l3, = plt.plot(x, y3, markersize=2)
plt.legend([l0,l1,l2,l3],["bubblesort","Fibonacci", "naive factorial", "dot product"],loc="upper left")
plt.savefig("data.png")
