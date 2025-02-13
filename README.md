# Suda

This README file provides instructions for reproducing the experimental results in the paper "Suda: An Efficient and Secure Unbalanced Data Alignment Framework for Vertical Privacy-Preserving Machine Learning" (Usenix Security 2025).


# Requerments:

## Hardware and software dependencies
Intel(R) Xeon(R) Platinum 8260 ++

memory 500 GB ++

Debian GNU/Linux 10 (buster) ++  

gcc 11.5.0++ 

clang 18.1.8++

## Third-party libraries 

```
cd src/third_party
bash gmp.get
bash ntl.get
bash libsodium.get
bash seal.get
bash pailliercryptolib.get
bash volepsi.get
```

## Setup:

```
bash setup.sh
```

# Efficiency of secure unbalanced data alignment

Evaluate the efficiency of Suda over different data settings. The results are illustrated in part of Table 1 (secure data alignment part), Table 2, Table 3 and Table 4 in the paper.

# Part of Table 1 (secure data alignment part)


Firstly, download and preprocess the datasets:

```
bash preprocess_dataset.sh
```

Then run the following script:

```
nohup bash run_psi_to_share_using_files_test.sh > run_psi_to_share_using_files_test.log 2>&1 &
```

# Table 2 

Run the following script:

```
bash run_psi_to_share_test_size.sh > run_psi_to_share_test_size.log 2>&1 &
```

# Table 3

Run the following script:

```
bash run_psi_to_share_test_payload.sh > run_psi_to_share_test_payload.log 2>&1 &
```

# Table 4

Run the following script:

```
bash run_psi_to_share_test_interratio.sh > run_psi_to_share_test_interratio.log 2>&1 &
```

# Efficiency of secure training

Evaluate the performance of secure training using the outputs of secure data alignment. 
The results are illustrated in part of Table 1 (secure training part).

## Part of Table 1 (secure training part)

After running the scripts in Part of Table 1 (secure data alignment part), run the following script:

```
bash run_mpclr.sh
```

# Efficiency of batch PIR


## Table 5

Run the following script:

```
nohup bash run_batchpir_test.sh >batch_pir.log  2>&1 &
```

