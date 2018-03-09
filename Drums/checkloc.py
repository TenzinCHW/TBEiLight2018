import matplotlib.pyplot as plt
import config

x = [xy[0] for xy in config.lamps.values()]
y = [xy[1] for xy in config.lamps.values()]
plt.plot(x, y, 'bs')
plt.xlabel('x')
plt.ylabel('y')
plt.show()

