#!/usr/bin/env python3

# pyre-unsafe

# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

import tempfile

import crypten
import crypten.communicator as comm
import torch
import torch.nn as nn
import torch.nn.functional as F
from crypten_utils.util import NoopContextManager
from torchvision import datasets, transforms
from heart_disease_data import heart_disease_data

# 统计通信量
from crypten.config import cfg
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
    num_samples=700,
    data=None,
):
    """
    Args:
        context_manager: used for setting proxy settings during download.
    """
    crypten.init()

    x_train = data.x_train
    train_labels = data.y_train

    n_feature_alice = 4
    n_features = x_train.shape[1]
    n_feature_bob = n_features - n_feature_alice
    data_alice, data_bob = torch.split(x_train, [n_feature_alice, n_feature_bob], dim=1)

    # assumes at least two parties exist
    # broadcast dummy data with same shape to remaining parties
    rank = comm.get().get_rank()
    if rank == 0:
        x_alice = data_alice
    else:
        x_alice = torch.empty(data_alice.size())

    if rank == 1:
        x_bob = data_bob
    else:
        x_bob = torch.empty(data_bob.size())

    # encrypt
    x_alice_enc = crypten.cryptensor(x_alice, src=0)
    x_bob_enc = crypten.cryptensor(x_bob, src=1)

    # combine feature sets
    x_combined_enc = crypten.cat([x_alice_enc, x_bob_enc], dim=1)

    # reduce training set to num_samples
    x_reduced = x_combined_enc[:num_samples]
    y_reduced = train_labels[:num_samples]

    # encrypt plaintext model
    model = LogisticRegressionModel(n_features).encrypt()

    # encrypted training
    train_encrypted(
        x_reduced, y_reduced, model, num_epochs, learning_rate, batch_size, print_freq
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
            y_train = crypten.cryptensor(y_encrypted[start:end], requires_grad=True)

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
        correct = torch.abs(y_encrypted[start:end] - pred) < 0.5
        accuracy = correct.float().mean()
        loss_plaintext = loss_value.get_plain_text().item()
        print(
            f"Epoch {epoch} completed: "
            f"Loss {loss_plaintext:.4f} Accuracy {accuracy.item():.2f}"
        )