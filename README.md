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


# Run expierments in Table 2

```
bash run_psi_to_share_test_size.sh > run_psi_to_share_test_size.log 2>&1 &
```

# Run expierments in Table 3

```
bash run_psi_to_share_test_payload.sh > run_psi_to_share_test_payload.log 2>&1 &
```

# Run expierments in Table 4

```
bash run_psi_to_share_test_interratio.sh > run_psi_to_share_test_interratio.log 2>&1 &
```


# Run expierments in Table 5

```
nohup bash run_batchpir_test.sh >batch_pir.log  2>&1 &
```

# Run expierments in Table 1 (Suda)

## Part Secure Data Alignment 

Firstly, download and preprocess the datasets to needed formula:

```
bash preprocess_dataset.sh
```

Then you can run psi to share using preprocessed datasets:

```
nohup bash run_psi_to_share_using_files_test.sh > run_psi_to_share_using_files_test.log 2>&1 &
```

## Part Secure Training

Finnaly, you can run mpc-lr on the aliged secret-share dataset:

```
bash run_mpclr.sh
```
