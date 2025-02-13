import torch
from heart_disease_data import heart_disease_data

class LR(torch.nn.Module):

    def __init__(self, n_features):
        super(LR, self).__init__()
        self.lr = torch.nn.Linear(n_features, 1)
        
    def forward(self, x):
        out = torch.sigmoid(self.lr(x))
        return out


def train(model: LR, optim, criterion, x, y, epochs):
    for e in range(1, epochs + 1):
        optim.zero_grad()
        out = model(x)
        loss = criterion(out, y)
        loss.backward()
        optim.step()
        print(f"Loss at epoch {e}: {loss.data}")
    return model

def accuracy(model, x, y):
    out = model(x)
    print("out = ", out)
    print("y = ", y)
    correct = torch.abs(y - out) < 0.5
    return correct.float().mean()


if __name__ == "__main__":
      x_train = heart_disease_data.x_train
      print(x_train.shape)
      y_train = heart_disease_data.y_train
      n_features = x_train.shape[1]
      model = LR(n_features)
      
      optim = torch.optim.SGD(model.parameters(), lr=0.1)
      # use Binary Cross Entropy Loss
      criterion = torch.nn.BCELoss()
      # define the number of epochs for both plain and encrypted training
      EPOCHS = 50
      model = train(model, optim, criterion, x_train, y_train, EPOCHS)
      
      x_test = heart_disease_data.x_test
      y_test = heart_disease_data.y_test
      plain_accuracy = accuracy(model, x_test, y_test)
      print(f"Accuracy on plain test_set: {plain_accuracy}")



