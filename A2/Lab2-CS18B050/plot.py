import matplotlib.pyplot as plt
import numpy as np
import sys
b = np.loadtxt(sys.argv[1])
plt.plot(b)
plt.xlabel("Update Number")
plt.ylabel("Congestion Window size in kB")
plt.title("Congestion Window size vs Update Number")
ymax = 140
bmax = np.max(b)
if bmax > 800:
    ymax = 1200
elif bmax > 600:
    ymax = 800
elif bmax > 400:
    ymax = 600
elif bmax > 140:
    ymax = 400
plt.ylim(0,ymax)
plt.savefig(sys.argv[2])
#plt.show()
