import random
import torch

import pandas as pd

class HeartDiseaseData(object):
    def __init__(self):
        x_train, y_train, x_test, y_test = self.heart_disease_data()
        self.x_train = x_train
        self.y_train = y_train
        self.x_test = x_test
        self.y_test = y_test
    
    @staticmethod
    def split_train_test(x, y, test_ratio=0.3):
        idxs = [i for i in range(len(x))]
        random.shuffle(idxs)
        # delimiter between test and train data
        delim = int(len(x) * test_ratio)
        test_idxs, train_idxs = idxs[:delim], idxs[delim:]
        return x[train_idxs], y[train_idxs], x[test_idxs], y[test_idxs]

    def heart_disease_data(self):
        data = pd.read_csv("./data/framingham.csv")
        # drop rows with missing values
        data = data.dropna()
        # drop some features
        data = data.drop(columns=["education", "currentSmoker", "BPMeds", "diabetes", "diaBP", "BMI"])
        # balance data
        grouped = data.groupby('TenYearCHD')
        data = grouped.apply(lambda x: x.sample(grouped.size().min(), random_state=73).reset_index(drop=True))
        # extract labels
        y = torch.tensor(data["TenYearCHD"].values).float().unsqueeze(1)
        data = data.drop(columns=["TenYearCHD"])
        # standardize data
        data = (data - data.mean()) / data.std()
        x = torch.tensor(data.values).float()
        return HeartDiseaseData.split_train_test(x, y)
  

heart_disease_data = HeartDiseaseData()
print(heart_disease_data.x_train)