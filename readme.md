# Requerments:

Debian GNU/Linux 10 (buster) +,    gcc 11.5.0+,  64GB Memory.

If you wish run  secure data align on a big Server's dataset, for example [1<<24, 1000],  or on the benchmark datasets i.e CFI dataset and SVHN dataset,  big memory (about 128GB) is recommend.

# Install dependencies

```
cd src/third_party
bash gmp.get
bash ntl.get
bash libsodium.get
bash seal.get
bash pailliercryptolib.get
bash volepsi.get

```

If meet the error like

```
CMake Error at .../suda/volepsi/out/coproto/thirdparty/getBoost.cmake:67 (file):
file DOWNLOAD HASH mismatch
for file: [.../suda/volepsi/out/boost_1_86_0.tar.bz2]
expected hash: [1bed88e40401b2cb7a1f76d4bab499e352fa4d0c5f31c0dbae64e24d34d7513b]
actual hash: [79e6d3f986444e5a80afbeccdaf2d1c1cf964baa8d766d20859d653a16c39848]
```

in running `bash volepsi.get` , replace the line 8 in getBoost.cmake to

```
set(URL "https://archives.boost.io/release/1.86.0/source/boost_1_86_0.tar.bz2")
```

and re-run

```
bash volepsi.get
```

# Setup

```
bash setup.sh
```

# Batch pir:

```
./build/bin/batch_pir_protocol_test $host_log_n_data $batch_size $payload_size
```

$host_log_n_data:   the log2 number of records in Server ,

$batch_size: the number of client ids

$payload_size:  the payload size of every record ( in Byte)

for example:

```
./build/bin/batch_pir_protocol_test 20 1024 100
```

it means:

the number of records in Server  is 1<<20,

the payload size of every record is 100 byte
the number of client ids is 1024

# Secure data align:

```
./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 0  &
./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 1 
```

for example:

```
./build/bin/psi_to_share_test 20 1024 8 0  &
./build/bin/psi_to_share_test 20 1024 8 1
```

where

the size of Server's dataset is [1<<20, 8]

the number of Client's ids is 1024

# Secure data align with different intersection ratio:

```
./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 0 $inter_ratio & 
./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 1 $inter_ratio
```

where  `$inter_ratio`  means the ration of intersection, i.e

record number in intersection / record number of client

for example:

```
./build/bin/psi_to_share_test 20 4096 8 0  0.5 &
./build/bin/psi_to_share_test 20 4096 8 1  0.5
```

where

the size of Server's dataset is [1<<20, 8]

the number of Client's ids is 1024

the intersection ratio is 0.5

# Secure data align using files as inputs/outputs :

Firstly, download and preprocess the datasets to needed formula:

```
cd python
bash preprocess_dataset.sh
```

Then you can run psi to share  using preprocessed datasets:

```
./build/bin/psi_to_share_using_files_test 0 $server_input_file $server_output_file &
./build/bin/psi_to_share_using_files_test 1 $client_input_file $client_output_file 
```

for example:

a). Secure data align for  CFI dataset:

```
./build/bin/psi_to_share_using_files_test 0 ./data/CFIX.json  ./CFIX_output_file.cs0 &
./build/bin/psi_to_share_using_files_test 1 ./data/CFIY1024.json ./CFIX_output_file.cs1

```

b). Secure data align for  SVHN dataset:

```
./build/bin/psi_to_share_using_files_test 0 ./data/SVHNX.json  ./SVHN_output_file.cs0 &
./build/bin/psi_to_share_using_files_test 1 ./data/SVHNY1024.json ./SVHN_output_file.cs1 

```

# run mpc lr using alined dataset

To run mpc lr using alined dataset, use the command:

```
python3 ./mpc_lr/launcher.py -f0 [alined_output_file0] -f1 [alined_output_file1] --epochs [epochs]
```

where:

-f0:  the file name of alined output of Server

-f1:  the file name of alined output of client

--epochs: the training epochs

for example:

```
python3 ./mpc_lr/launcher.py -f0 CFIX_output_file.cs0 -f1 CFIX_output_file.cs1 --epochs 100
```
