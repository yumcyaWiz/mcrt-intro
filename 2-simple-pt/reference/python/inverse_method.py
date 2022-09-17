import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

if __name__ == "__main__":
    N = 10000
    u = np.random.random(N)
    x = -np.log(1 - u)
    sns.distplot(x)
    plt.savefig("inverse_method.png")
