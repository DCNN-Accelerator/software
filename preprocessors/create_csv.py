# Simple Python script to create a kernel csv file 

import pandas as pd
import numpy as np 

test_arr = np.random.rand(7,7)

print(test_arr.size)

pd.DataFrame(test_arr).to_csv('test_kernel.csv',header=None,index=None)
