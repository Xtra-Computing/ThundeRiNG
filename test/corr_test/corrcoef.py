from numpy.random import randn
from numpy.random import seed
from numpy import cov
from scipy.stats import pearsonr

from statsmodels.graphics import tsaplots

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from scipy.stats import spearmanr


from datetime import datetime

tmp1 = pd.read_csv("seq_1.csv",header = None,delimiter='\n')
tmp2 = pd.read_csv("seq_2.csv",header = None,delimiter='\n')

# prepare data
data1 = tmp1[0].to_numpy()/float(0xFFFFFFFF) - 0.5
data2 = tmp2[0].to_numpy()/float(0xFFFFFFFF) - 0.5






fig, [ax1, ax2] = plt.subplots(2, 1, sharex=True)
#262143
ax1.xcorr(data1, data2, usevlines=True, maxlags=4096, normed=True, lw=2)
ax1.grid(True)

ax2.acorr(data1, usevlines=True, normed=True, maxlags=4096, lw=2)
ax2.grid(True)

now = datetime.now() # current date and time

name = now.strftime("%m_%d_%Y_%H%M%S")

name = name + ".png"

plt.savefig(name)

