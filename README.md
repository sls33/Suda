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
cmake -S . -B build
cmake --build build -j
```

When executing this script, you may face an error about 'pip install sklearn' and you can resolve it by following the solution in https://github.com/facebookresearch/CrypTen/issues/512.

# Efficiency of secure unbalanced data alignment

Evaluate the efficiency of Suda over different data settings. The results are illustrated in Table 2, Table 3, Table 4 and part of Table 1 (secure data alignment part) in the paper.

## Table 2 

Run the following script:

```
bash run_psi_to_share_test_size.sh
```

The running results are stored in the result/data_size_ps.txt and result/data_size_pc.txt.

## Table 3

Run the following script:

```
bash run_psi_to_share_test_payload.sh 
```

The running results are stored in the result/feature_dimension_ps.txt and result/feature_dimension_pc.txt.

## Table 4

Run the following script:

```
bash run_psi_to_share_test_interratio.sh 
```

The running results are stored in the result/intersection_size_ps.txt and result/intersection_size_pc.txt.

## Part of Table 1 (secure data alignment part)

Firstly, download and preprocess the datasets:

```
cd python
bash preprocess_dataset.sh
```

Then run the following script:

```
bash run_psi_to_share_using_files_test.sh
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
bash run_batchpir_test.sh
```

The running results are stored in the result/batch_pir.txt.

