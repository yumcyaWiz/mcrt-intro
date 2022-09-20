import numpy as np
import matplotlib.pyplot as plt


def f(x):
    return np.sin(x)**2


if __name__ == "__main__":
    N = 10000
    u = np.random.random(N)
    x = np.pi * u
    pdf = 1 / np.pi

    n = np.arange(N) + 1
    integral = (f(x) / pdf).cumsum() / n

    plt.plot(n, integral)
    plt.axhline(0.5 * np.pi, c="black")
    plt.xlabel(r"$N$")
    plt.ylabel(r"$\frac{1}{N}\sum_{i=1}^N\frac{f(x_i)}{p(x_i)}$")
    plt.grid()
    plt.savefig("mcintegral.png")
