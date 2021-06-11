import popcount
import numpy as np
import matplotlib.pyplot as plt

seq_num =  4096
tmp = 1;
res = np.zeros(seq_num)
for i in range(seq_num):
    tmp = 0x5b5fee90a1001dcd * (i + 4096) +   0x219b40fff5054705;
    res[i] = popcount.hw(tmp)

plt.hist(res,64)
plt.show()