import matplotlib.pyplot as plt
import numpy as np
import sys
x = [0.001, 0.021, 0.041, 0.061, 0.081, 0.1]
y1 = [1.01333, 1.08, 1.1, 1.15333, 1.26667, 1.33333]
y2 = [1.00667, 1.04, 1.04667, 1.07333, 1.11333, 1.13333]
fig, ax = plt.subplots()
l1 = ax.plot(x,y1, label="GBN")
l2 = ax.plot(x,y2, label="SR")
plt.xlabel("Packet Drop Prob")
plt.ylabel("Retransission Ratio")
plt.title("Retransission Ratio vs Packet Drop Prob")
ax.legend()
#plt.show()
plt.savefig("plot.png")
