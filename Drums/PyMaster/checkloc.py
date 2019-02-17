import matplotlib.pyplot as plt
import config

x = [xy[0] for xy in config.lamps.values()]
y = [xy[1] for xy in config.lamps.values()]
dx = [dxy[0] for dxy in config.drum_loc]
dy = [dxy[1] for dxy in config.drum_loc]
plt.plot(x, y, 'bs', dx, dy, 'g^')
plt.xlabel('x')
plt.ylabel('y')
plt.show()

