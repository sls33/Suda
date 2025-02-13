import torchvision
from torchvision import datasets
import numpy as np
import struct

# int8_data = [[1,2],[3,4]]
# def to_bin_file(uint8_data, file_name):
#     byte_data = bytes(np.array(uint8_data, dtype="uint8"))
#     # # 将bytes对象写入二进制文件
#     with open(file_name, 'wb') as file:
#         file.write(byte_data)
        
        





download = True

# 下载SVHN数据集
svhn_train = datasets.SVHN(root='../data', split='train', download=download)
svhn_test = datasets.SVHN(root='../data', split='test', download=download)
svhn_extra = datasets.SVHN(root='../data', split='extra', download=download)



print(np.shape(svhn_train.data))
print(np.shape(svhn_test.data))
print(np.shape(svhn_extra.data))

# uint8_data = np.concatenate([svhn_train.data, svhn_test.data, svhn_extra.data], axis=0)
# print(np.shape(uint8_data))

# to_bin_file(uint8_data, "SVHN_X.bin")



print(np.shape(svhn_train.labels))
print(np.shape(svhn_test.labels))
print(np.shape(svhn_extra.labels))

# uint8_data = np.concatenate([svhn_train.labels, svhn_test.labels, svhn_extra.labels], axis=0)
# print(np.shape(uint8_data))
# to_bin_file(uint8_data, "SVHN_Y.bin")





