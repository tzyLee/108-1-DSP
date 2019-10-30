import matplotlib.pyplot as plt

x = [50, 100, 150, 200, 250, 300, 350, 400, 450]
y = [2057, 2150, 2122, 2141, 2166, 2172, 2174, 2173, 2172]
y = [i/2500 for i in y]
plt.plot(x, y)
plt.show()
