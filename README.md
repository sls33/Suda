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

When executing this script, you may face an error about 'pip install sklearn', and you can resolve it by setting this env variable to true

```
export SKLEARN_ALLOW_DEPRECATED_SKLEARN_PACKAGE_INSTALL=True
```

# Basic Test

After installing all the dependencies and third-party libraries, you can run the following command to run a simple functionality test. In this command, 20 refers to the larger data size $$N = 2^{20}$$, 1024 refers to the smaller data size $$n = 1024$$, and 100 refers to the feature dimensions $$m = 100$$. The results are stored in the files ``test_ps.txt`` and ``test_pc.txt``.

```
./build/bin/psi_to_share_test 20 1024 100 0 test_ps.txt & ./build/bin/psi_to_share_test 20 1024 100 1 test_pc.txt
```



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

The running results are stored in the result/CFIX_ps.txt, result/CFIX_pc.txt for the Character Font Images dataset, and result/SVHN_ps.txt, result/SVHN_pc.txt for the SVHN dataset.

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

