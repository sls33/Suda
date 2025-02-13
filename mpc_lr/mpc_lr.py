import crypten
try:
    # 初始化CrypTen
    crypten.init()
except Exception as e:
    raise e
import torch
import crypten.communicator as comm

from heart_disease_data import heart_disease_data


# 定义逻辑回归模型
class LogisticRegressionModel(crypten.nn.Module):
    def __init__(self, n_features):
        super(LogisticRegressionModel, self).__init__()
        self.lr = crypten.nn.Linear(n_features, 1)

    def forward(self, x):
        out = self.lr(x).sigmoid()
        return out
    

def train(model: LogisticRegressionModel, criterion, x, y, learnning_rate, epochs):
    for e in range(1, epochs + 1):
        model.zero_grad()
        out = model(x)
        print("out.shape = ", out.shape)
        loss = criterion(out, y)
        loss.backward()
        model.update_parameters(learnning_rate)
        print(f"Loss at epoch {e}: {loss.get_plain_text()}")  # 使用get_plain_text()获取明文损失值
    return model

def accuracy(model, x, y):
    out = model(x).get_plain_text()
    correct = torch.abs(y - out) < 0.5
    return correct.float().mean()


if __name__ == "__main__":
    x_train = heart_disease_data.x_train
    print(x_train)
    y_train = heart_disease_data.y_train
    n_features = x_train.shape[1]
    
    x_train_enc = crypten.cryptensor(x_train, src=0)  # 假设数据来自party 0
    print(x_train_enc)
    y_train_enc = crypten.cryptensor(y_train, src=0)  # 假设标签来自party 0

    epochs = 5
    learnning_rate = 0.1
    criterion = crypten.nn.BCELoss()
    # optimizer = crypten.optim.SGD(model.parameters(), lr=1)
    
    # 加密模型
    model = LogisticRegressionModel(n_features).encrypt()
    model = train(model, criterion, x_train_enc, y_train_enc, learnning_rate, epochs)
    rank = comm.get().get_rank()
    print(rank)
    
    
    x_test = heart_disease_data.x_test
    y_test = heart_disease_data.y_test
    x_test_enc = crypten.cryptensor(x_test, src=0)  # 假设数据来自party 0
    
    mpc_accuracy = accuracy(model, x_test_enc, y_test)
    print(f"Accuracy on plain test_set: {mpc_accuracy}")