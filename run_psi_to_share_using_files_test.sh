
## nohup bash run_psi_to_share_using_files_test.sh > run_psi_to_share_using_files_test.log 2>&1 &
sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: htb default 1
sudo tc class add dev lo parent 1: classid 1:1 htb rate 75Mbit
sudo tc qdisc add dev lo parent 1:1 handle 2:0 netem delay 20ms



./build/bin/psi_to_share_using_files_test 0 ./data/CFIX.json  ./CFIX_output_file.cs0 result/CFIX_ps.txt &
./build/bin/psi_to_share_using_files_test 1 ./data/CFIY1024.json ./CFIX_output_file.cs1 result/CFIX_pc.txt



./build/bin/psi_to_share_using_files_test 0 ./data/SVHNX.json  ./SVHN_output_file.cs0 result/SVHN_ps.txt&
./build/bin/psi_to_share_using_files_test 1 ./data/SVHNY1024.json ./SVHN_output_file.cs1 result/SVHN_pc.txt



sudo tc qdisc del dev lo root