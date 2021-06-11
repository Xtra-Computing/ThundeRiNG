#!/usr/bin/python3

import sys

print('Number of arguments:', len(sys.argv), 'arguments.')
print('Argument List:', str(sys.argv))

from numpy.random import randn
from numpy.random import seed
from numpy import cov
from scipy.stats import pearsonr

from statsmodels.graphics import tsaplots

from scipy.stats import spearmanr

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


from scipy.stats import spearmanr
from scipy.stats import kendalltau
from scipy.stats import pearsonr


from datetime import datetime

tmp1 = pd.read_csv("seq_1.csv",header = None,delimiter='\n')
tmp2 = pd.read_csv("seq_2.csv",header = None,delimiter='\n')

# prepare data
data1 = tmp1[0].to_numpy()
data2 = tmp2[0].to_numpy()

coef, p = spearmanr(data1, data2)
print('Spearmans correlation coefficient: %.3f' % coef)
# interpret the significance
alpha = 0.05
if p > alpha:
    print('Samples are uncorrelated (fail to reject H0) p=%.3f' % p)
else:
    print('Samples are correlated (reject H0) p=%.3f' % p)


corr, _ = pearsonr(data1, data2)
print('Pearsons correlation: %.9f' % corr)

coef, p = spearmanr(data1, data2)
print('Spearmans correlation coefficient: %.9f' % coef)
# interpret the significance
alpha = 0.05
if p > alpha:
    print('Samples are uncorrelated (fail to reject H0) p=%.3f' % p)
else:
    print('Samples are correlated (reject H0) p=%.3f' % p)


coef, p = kendalltau(data1, data2)
print('Kendall correlation coefficient: %.9f' % coef)
# interpret the significance
alpha = 0.05
if p > alpha:
    print('Samples are uncorrelated (fail to reject H0) p=%.3f' % p)
else:
    print('Samples are correlated (reject H0) p=%.3f' % p)

data1 = tmp1[0].to_numpy()/float(0xFFFFFFFF) - 0.5
data2 = tmp2[0].to_numpy()/float(0xFFFFFFFF) - 0.5


#fig, [ax2, ax1] = plt.subplots(2, 1, sharex=True)
##262143
#ax1.xcorr(data1, data2, usevlines=True, maxlags=4096, normed=True, lw=2)
#ax1.grid(True)
#
#ax2.acorr(data1, usevlines=True, normed=True, maxlags=4096, lw=2)
#ax2.grid(True)
#
#now = datetime.now() # current date and time
#
#name = now.strftime("%m_%d_%Y_%H%M%S")
#
#name = name + ".pdf"
#
#plt.savefig(name)
#
