#!/usr/bin/env python3

# pyre-unsafe

# Copyright (c) Facebook, Inc. and its affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.
"""
To run mpc_autograd_cnn example:

$ python examples/mpc_autograd_cnn/launcher.py

To run mpc_linear_svm example on AWS EC2 instances:

$ python3 scripts/aws_launcher.py \
      --ssh_key_file=$HOME/.aws/fair-$USER.pem \
      --instances=i-038dd14b9383b9d79,i-08f057b9c03d4a916 \
      --aux_files=examples/mpc_autograd_cnn/mpc_autograd_cnn.py \
      examples/mpc_autograd_cnn/launcher.py
"""

import argparse
import logging
import os
import time
import psutil
# from heart_disease_data import heart_disease_data

from crypten_utils.multiprocess_launcher import (
    MultiProcessLauncher,
)


parser = argparse.ArgumentParser(description="CrypTen Autograd CNN Training")


def validate_world_size(world_size):
    world_size = int(world_size)
    if world_size < 2:
        raise argparse.ArgumentTypeError(f"world_size {world_size} must be > 1")
    return world_size


parser.add_argument(
    "--world_size",
    type=validate_world_size,
    default=2,
    help="The number of parties to launch. Each party acts as its own process",
)
parser.add_argument(
    "--epochs", default=10, type=int, metavar="N", help="number of total epochs to run"
)
parser.add_argument(
    "--lr",
    "--learning-rate",
    default=0.1,
    type=float,
    metavar="LR",
    help="initial learning rate",
)
parser.add_argument(
    "-b",
    "--batch-size",
    default=128,
    type=int,
    metavar="N",
    help="mini-batch size (default: 128)",
)
parser.add_argument(
    "--print-freq",
    "-p",
    default=5,
    type=int,
    metavar="PF",
    help="print frequency (default: 5)",
)
# parser.add_argument(
#     "--num-samples",
#     "-n",
#     default=780,
#     type=int,
#     metavar="N",
#     help="num of samples used for training (default: 100)",
# )
parser.add_argument(
    "--file0",
    "-f0",
    type=str,
    default="../CFIX_output_file.cs0",
    help="filename for party0"
)
parser.add_argument(
    "--file1",
    "-f1",
    type=str,
    default="../CFIX_output_file.cs1",
    help="filename for party1"
)


def _run_experiment(args):
    level = logging.DEBUG
    # if "RANK" in os.environ and os.environ["RANK"] != "0":
    #     level = logging.CRITICAL
    logging.getLogger().setLevel(level)
    logging.basicConfig(
        level=level,
        format="%(asctime)s - %(process)d - %(name)s - %(levelname)s - %(message)s",
    )
    # pyre-fixme[21]: Could not find module `mpc_autograd_cnn`.
    # from mpc_autograd_cnn import run_mpc_autograd_cnn  # @manual

    # run_mpc_autograd_cnn(
    #     num_epochs=args.epochs,
    #     learning_rate=args.lr,
    #     batch_size=args.batch_size,
    #     print_freq=args.print_freq,
    #     num_samples=args.num_samples,
    # )
    # from mpc_autograd_lr_from_file import run_mpc_autograd_lr  # @manual
    from mpc_autograd_lr_from_csv import run_mpc_autograd_lr 

    filenames=[args.file0, args.file1]
    run_mpc_autograd_lr(
        num_epochs=args.epochs,
        learning_rate=args.lr,
        batch_size=args.batch_size,
        print_freq=args.print_freq,
        # num_samples=args.num_samples,
        # data=args.data,
        filenames=filenames
    )

def _init_monitor(launcher):
    process_ids = [process.pid for process in launcher.processes]
    monitor_map = {
        process.pid: {
            "cpu_usage": 0.0,
            "mem_usage(MB)": 0.0,
        }
        for process in launcher.processes
    }
    return process_ids, monitor_map

def _monitor_thread(process_ids, monitor_map):
    while any([psutil.pid_exists(pid) for pid in process_ids]):
        for pid in process_ids:
            if psutil.pid_exists(pid):
                current_process = psutil.Process(pid)
                rss = current_process.memory_info().rss
                cpu_percent = current_process.cpu_percent(interval=0.1)
                for child in current_process.children(recursive=True):
                    rss += child.memory_info().rss
                    cpu_percent += child.cpu_percent()
                rss = rss / 1024 / 1024
                monitor_map[pid]["cpu_usage"] = max(cpu_percent, monitor_map[pid]["cpu_usage"])
                monitor_map[pid]["mem_usage(MB)"] = max(rss, monitor_map[pid]["mem_usage(MB)"])
        time.sleep(0.1)

def monitor(process_ids, monitor_map):
    try:
        import threading
        t = threading.Thread(target=_monitor_thread, args=(process_ids, monitor_map))
        t.setDaemon(True)
        t.start()
    except Exception as e:
        logging.error(f"Failed to monitor memory: {e}")

def main(run_experiment):
    args = parser.parse_args()
    # args.data = heart_disease_data
    # run multiprocess by default
    start=time.time()
    launcher = MultiProcessLauncher(args.world_size, run_experiment, args)
    launcher.start()
    process_ids, monitor_map = _init_monitor(launcher)
    monitor(process_ids, monitor_map)
    launcher.join()
    launcher.terminate()
    end=time.time()
    print("time=", end-start, "s")

    print(f"Monitoring usage: {monitor_map}")


if __name__ == "__main__":
    main(_run_experiment)
