#!/usr/bin/env python3

# pyre-unsafe

# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

import tempfile

import crypten
import crypten.communicator as comm
from crypten.mpc.ptype import ptype as Ptype
import torch
import torch.nn as nn
import torch.nn.functional as F
from crypten_utils.util import NoopContextManager
from torchvision import datasets, transforms
# from heart_disease_data import heart_disease_data
import pandas as pd

# 统计通信量
from crypten.config import cfg
from crypten.mpc import MPCTensor

cfg.communicator.verbose = True


class LogisticRegressionModel(crypten.nn.Module):
    def __init__(self, n_features):
        super(LogisticRegressionModel, self).__init__()
        self.lr = crypten.nn.Linear(n_features, 1)

    def forward(self, x):
        out = self.lr(x).sigmoid()
        return out


def run_mpc_autograd_lr(
    context_manager=None,
    num_epochs=3,
    learning_rate=0.001,
    batch_size=100,
    print_freq=5,
    filenames=[]
):
    """
    Args:
        context_manager: used for setting proxy settings during download.
    """
    # Specify file locations to save each piece of data
    # filenames = {
    #     "p0": "../CFIX_output_file.cs0",
    #     "p1": "../CFIX_output_file.cs1"
    # }

    crypten.init()

    # read mpc shares from file
    rank = comm.get().get_rank()
    if rank == 0:
        data = pd.read_csv(filenames[0], header=None) 
    else:
        data = pd.read_csv(filenames[1], header=None) 
    print("data=", data)
    feature = data.iloc[:,0:-1]
    label = data.iloc[:,-1:]
    feature = MPCTensor.from_shares(torch.tensor(feature.values))
    label = MPCTensor.from_shares(torch.tensor(label.values), ptype=Ptype.binary)
    label = label.to(ptype=Ptype.arithmetic)
    n_features = feature.shape[1]
    # encrypt plaintext model
    model = LogisticRegressionModel(n_features).encrypt()

    # encrypted training
    train_encrypted(
        feature, label, model, num_epochs, learning_rate, batch_size, print_freq
    )

    comm.get().print_communication_stats()


def train_encrypted(
    x_encrypted,
    y_encrypted,
    encrypted_model,
    num_epochs,
    learning_rate,
    batch_size,
    print_freq,
):
    rank = comm.get().get_rank()
    loss = crypten.nn.BCELoss()
    num_samples = x_encrypted.size(0)

    for epoch in range(num_epochs):
        last_progress_logged = 0
        # only print from rank 0 to avoid duplicates for readability
        if rank == 0:
            print(f"Epoch {epoch} in progress:")

        for j in range(0, num_samples, batch_size):
            # define the start and end of the training mini-batch
            start, end = j, min(j + batch_size, num_samples)

            # switch on autograd for training examples
            x_train = x_encrypted[start:end]
            x_train.requires_grad = True
            # y_one_hot = label_eye[y_encrypted[start:end]]
            y_train = y_encrypted[start:end]
            y_train.requires_grad = True

            # perform forward pass:
            output = encrypted_model(x_train)
            loss_value = loss(output, y_train)

            # backprop
            encrypted_model.zero_grad()
            loss_value.backward()
            encrypted_model.update_parameters(learning_rate)

            # log progress
            if j + batch_size - last_progress_logged >= print_freq:
                last_progress_logged += print_freq
                print(f"Loss {loss_value.get_plain_text().item():.4f}")

        # compute accuracy every epoch
        pred = output.get_plain_text()
        correct = torch.abs(y_train.get_plain_text() - pred) < 0.5
        accuracy = correct.float().mean()
        loss_plaintext = loss_value.get_plain_text().item()
        print(
            f"Epoch {epoch} completed: "
            f"Loss {loss_plaintext:.4f} Accuracy {accuracy.item():.2f}"
        )
