import os
import pandas as pd
import json
import numpy as np
import torchvision
from torchvision import datasets
import numpy as np
import struct



def csv_to_json_CFI(test_flag):
    save_as_cols = False
    save_as_1row = False
    save_as_rows = False

    # 设置目录路径
    directory = '../data/character+font+images'

    # 读取目录下的所有csv文件
    all_data = []
    file_num = 0
    for filename in os.listdir(directory):
        print("file_num=", file_num)
        if filename.endswith('.csv'):
            file_path = os.path.join(directory, filename)
            data = pd.read_csv(file_path)  # 读取csv文件
            data = data.iloc[:,3:]
            all_data.append(data)
        file_num +=1
        if test_flag and file_num>4:
            break

    print("load OK")
    # 将所有数据整合为一个数据框
    combined_data = pd.concat(all_data, ignore_index=True)
    print("concat OK")
    combined_data['italic']=combined_data['italic'].astype(bool)

    print(combined_data)
    print(combined_data.shape)


    feature = combined_data.iloc[:,[0] + list(range(2, len(combined_data.columns)))]
    feature['strength'] = (feature['strength']*(1<<16)).astype(int)
    feature['orientation'] = (feature['orientation']*(1<<16)).astype(int)
    print("feature:", feature.shape)

    label = combined_data.iloc[:,1:2]
    # label.reset_index(inplace=True)
    print("label:", label.shape)
    
    for num in [4096, 2048, 1024]:
        label = label.sample(num)
        label_ = label.sample(frac=1.0)
        random_pm1 = (np.random.random(size=[num])>0.2).astype('int')*2-1
        label_.set_index(label_.index*random_pm1, inplace=True)
        
        label_dict = {"index": label_.index.tolist(), "label": label_.iloc[:,0].tolist()}
        print("label_dict=", label_dict)
        with open('../data/CFIY{}.json'.format(num), 'w') as file:
            json.dump(label_dict, file)
        print("label_dict{} write OK".format(num))

    if save_as_cols:
        # 将 DataFrame 转换为 JSON 字符串
        feature_dict = feature.to_dict(orient='list', index=True)  
        index_list = feature.index.tolist()
        feature_dict['index'] = index_list
        # 将 DataFrame 转换为 JSON 数组
        print("feature_dict OK")
        # print(json_dict)
        # # 将 JSON 字符串写入文件
        if test_flag:
            file_name='../data/CFIX_small.json'
        else:
            file_name = '../data/CFIX.json'
        with open(file_name, 'w') as file:
            json.dump(feature_dict, file)
        print("feature write OK")
     

    if save_as_1row:
        data_list = np.reshape(feature.values, [-1]).tolist()
        index_list = feature.index.tolist()
        # print("index_list=", index_list)
        feature_dict = {"index": index_list, "data": data_list}
        if test_flag:
            file_name = '../data/CFIX_1row_small.json'
        else:
            file_name = '../data/CFIX_1row.json'
        with open(file_name, 'w') as file:
            json.dump(feature_dict, file)
        print("feature_dict write OK")
    if save_as_rows:
        data_list = feature.values.tolist()
        index_list = feature.index.tolist()
        # print(data_list[0])
        # print("index_list=", index_list)
        feature_dict = {"index": index_list, "data": data_list}
        with open('../data/CFIX_rows.json', 'w') as file:
            json.dump(feature_dict, file)
        print("feature_dict write OK")
        


def get_SVHN(test_flag):
    download = False
    save_as_cols = False
    save_as_1row = False
    save_as_rows = False
    # 下载SVHN数据集
    svhn_test = datasets.SVHN(root='../data', split='test', download=download)
    if not test_flag:
        svhn_train = datasets.SVHN(root='../data', split='train', download=download)
        svhn_extra = datasets.SVHN(root='../data', split='extra', download=download)
    
    if test_flag:
        feature = svhn_test.data
        feature = np.reshape(feature, [feature.shape[0], -1])
        feature = pd.DataFrame(feature)
        label = svhn_test.labels
        label = pd.DataFrame(label%2).astype(bool)
    else:
        feature = np.concatenate([svhn_train.data, svhn_test.data, svhn_extra.data], axis=0)
        feature = np.reshape(feature, [feature.shape[0], -1])
        feature = pd.DataFrame(feature)
        label = np.concatenate([svhn_train.labels, svhn_test.labels, svhn_extra.labels], axis=0)
        label = pd.DataFrame(label%2).astype(bool)
    print("feature=",feature)
    print("label=", label)

    for num in [4096, 2048, 1024]:
        label = label.sample(num)
        label_ = label.sample(frac=1.0)
        # label_.index[int(num*0.8):num]=-label_.index[int(num*0.8):num]
        random_pm1 = (np.random.random(size=[num])>0.2).astype('int')*2-1
        label_.set_index(label_.index*random_pm1, inplace=True)
        
        label_dict = {"index": label_.index.tolist(), "label": label_.iloc[:,0].tolist()}
        # print("label_dict=", label_dict)
        with open('../data/SVHNY{}.json'.format(num), 'w') as file:
            json.dump(label_dict, file)
        print("label_dict{} write OK".format(num))

    if save_as_cols:
        # 将 DataFrame 转换为 JSON 字符串
        feature_dict = feature.to_dict(orient='list', index=True)  
        feature_dict['index']=feature.index.tolist()
        print("feature_dict OK")
        # print(json_dict)
        # # 将 JSON 字符串写入文件
        if test_flag:
            file_name = '../data/SVHNX_small.json'
        else:
            file_name = '../data/SVHNX.json'
        with open(file_name, 'w') as file:
            json.dump(feature_dict, file)
        print("feature write OK")
     
       
    if save_as_1row:
        data_list = np.reshape(feature.values, [-1]).tolist()
        index_list = feature.index.tolist()
        # print("index_list=", index_list)
        feature_dict = {"index": index_list, "data": data_list}
        if test_flag:
            file_name = '../data/SVHNX_1row_small.json'
        else:
            file_name = '../data/SVHNX_1row.json'
        with open(file_name, 'w') as file:
            json.dump(feature_dict, file)
        print("feature_dict write OK")
    if save_as_rows:
        data_list = feature.values.tolist()
        index_list = feature.index.tolist()
        # print(data_list[0])
        # print("index_list=", index_list)
        feature_dict = {"index": index_list, "data": data_list}
        with open('../data/SVHNX_rows.json', 'w') as file:
            json.dump(feature_dict, file)
        print("feature_dict write OK")
  



if __name__ =='__main__':
    csv_to_json_CFI(test_flag=False)
    get_SVHN(test_flag=False)