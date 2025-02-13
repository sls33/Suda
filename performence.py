import re
import pandas as pd
import sys
import numpy as np



def performance_batch_pir(log_file, performance_file):
    # 读取日志文件
    with open(log_file, 'r') as file:
        log_content = file.read()

    # 定义要提取的关键字
    # keywords = ['num_payload=', 'num_query=', 'payload_size=', 'server_encode_time:', 
    #             'Mem Usage of Server initialize=', 'client_init_time:', 'Mem Usage of Client initialize=',
    #             'Gen query time: ', 'Mem Usage of Client generate query=', 'Gen response time: ',
    #             'Mem Usage of Gen response=', 'Extract answer time: ', 'Mem Usage of Extract answer=',
    #             'Query size: ', 'Response size: ']
    keywords = ['host_log_n_data=', 'batch_size=', 'payload_size=', 'server_init_time:', 'Gen response time: ', 'client_init_time:', 'Gen query time: ', 
                'Extract answer time: ',
                'Mem Usage of Server initialize=',  'Mem Usage of Gen response=', 
                'Mem Usage of Client initialize=',
                'Mem Usage of Client generate query=', 
                'Mem Usage of Extract answer=',
                'Query size:',
                'Response size:']

    # 初始化空字典存储提取的值
    data = {key: [] for key in keywords}

    # 使用正则表达式提取关键字对应的值
    for key in keywords:
        if key in ['host_log_n_data=', 'batch_size=']:
            # matches = re.findall(f'{key}(\d+\.?\d+?)', log_content)
            matches = re.findall(f'{key}(\d+\,)', log_content)
            matches = [r.replace(",", "") for r in matches]
        else:
            matches = re.findall(f'{key}(.*\n)', log_content)
            matches = [r.replace("\n", "") for r in matches]
            matches = [r.replace("ms", "") for r in matches]
            matches = [r.replace("MB", "") for r in matches]
            matches = [r.replace("KBytes", "") for r in matches]
            matches = [r.replace(" ", "") for r in matches]
        data[key] = matches
        # print(key)
        # print(len(matches))

    # 将提取的数据组成表格
    # print(data)
    df = pd.DataFrame(data)

    df["Mem usage of Server"]=df["Mem Usage of Server initialize="].apply(pd.to_numeric)+df["Mem Usage of Gen response="].apply(pd.to_numeric)
    df["Mem usage of Client"]=(df["Mem Usage of Client initialize="].apply(pd.to_numeric)+df["Mem Usage of Client generate query="].apply(pd.to_numeric)).combine(df["Mem Usage of Extract answer="].apply(pd.to_numeric), func=max)
    df.drop(["Mem Usage of Server initialize=", "Mem Usage of Gen response=", "Mem Usage of Client initialize=", "Mem Usage of Client generate query=", "Mem Usage of Extract answer="], axis=1, inplace=True)

    df["server time"]=df["server_init_time:"].apply(pd.to_numeric)+df["Gen response time: "].apply(pd.to_numeric)
    df["server time"]=df["server time"]/1000
    df["client time"]=df["client_init_time:"].apply(pd.to_numeric)+df["Gen query time: "].apply(pd.to_numeric)+df["Extract answer time: "].apply(pd.to_numeric)
    df["client time"]=df["client time"]/1000
    df['Query size:']=df['Query size:'].apply(pd.to_numeric)/1024
    df['Response size:']=df['Response size:'].apply(pd.to_numeric)/1024
    df.drop(["Gen response time: ", "client_init_time:", "Gen query time: ", "Extract answer time: "], axis=1, inplace=True)


    print(df)

    df.to_csv(performance_file)
    
    


def performance_psi_to_share(log_file, performance_file):
    # 读取日志文件
    with open(log_file, 'r') as file:
        log_content = file.read()

    # 定义要提取的关键字
    # keywords = ['num_payload=', 'num_query=', 'payload_size=', 'server_encode_time:', 
    #             'Mem Usage of Server initialize=', 'client_init_time:', 'Mem Usage of Client initialize=',
    #             'Gen query time: ', 'Mem Usage of Client generate query=', 'Gen response time: ',
    #             'Mem Usage of Gen response=', 'Extract answer time: ', 'Mem Usage of Extract answer=',
    #             'Query size: ', 'Response size: ']
    keywords = ['host_log_n_data=', 'batch_size=', 'payload_size=', 
                    'mem usage of server:', 'mem usage of client:',
                 'comm. from server to client:', 'comm. from client to server:',
                'total time of server=', 'total time of client=']

    # 初始化空字典存储提取的值
    data = {key: [] for key in keywords}

    # 使用正则表达式提取关键字对应的值
    for key in keywords:
        if key in ['host_log_n_data=', 'batch_size=']:
            # matches = re.findall(f'{key}(\d+\.?\d+?)', log_content)
            matches = re.findall(f'{key}(\d+\,)', log_content)
            matches = [r.replace(",", "") for r in matches]
        else:
            matches = re.findall(f'{key}(.*\n)', log_content)
            matches = [r.replace("\n", "") for r in matches]
            matches = [r.replace("ms", "") for r in matches]
            matches = [r.replace("MB", "") for r in matches]
            matches = [r.replace("KBytes", "") for r in matches]
            matches = [r.replace(" ", "") for r in matches]
        data[key] = matches
        # print(key)
        # print(len(matches))

    # 将提取的数据组成表格
    # print(data)
    df = pd.DataFrame(data, dtype='float32')
    df['time']=df['total time of server='].combine(df["total time of client="], func=max)
    df['time']=df['time']/1000
    df['comm.']=(df['comm. from server to client:']+df['comm. from client to server:'])/1024
    df.drop(['total time of server=', "total time of client=", 'comm. from server to client:', 'comm. from client to server:'], axis=1, inplace=True)


    print(df)

    df.to_csv(performance_file)
    
if __name__ == '__main__':
    if sys.argv[1]=="table2":
        performance_psi_to_share("run_psi_to_share_test_size.log", "performance_psi_to_share_test_size.csv")
    if sys.argv[1]=="table3":
        performance_psi_to_share("run_psi_to_share_test_payload.log", "performance_psi_to_share_test_payload.csv")
    if sys.argv[1]=="table4":
        performance_psi_to_share("run_psi_to_share_test_interratio.log", "performance_psi_to_share_test_interratio.csv")
    if sys.argv[1]=="table5":
        performance_batch_pir("batch_pir.log", "performance_batch_pir.csv")
    if sys.argv[1]=="table1":
        performance_psi_to_share("run_psi_to_share_using_files_test.log", "performance_psi_to_share_using_files.csv")

        

