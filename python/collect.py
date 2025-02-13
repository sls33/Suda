import pandas as pd
import numpy as np

# prime_num = 1337006139375617
data0 = pd.read_csv("../CFIX_output_file.cs0", dtype="int64", header=None)
data1 = pd.read_csv("../CFIX_output_file.cs1", dtype="int64", header=None)

# print(data0)
# print(data1)
data = (data0 + data1)
print(data)


data0 = pd.read_csv("../SVHN_output_file.cs0", dtype="int64", header=None)
data1 = pd.read_csv("../SVHN_output_file.cs1", dtype="int64", header=None)

# print(data0)
# print(data1)
data = (data0 + data1)


print(data)


