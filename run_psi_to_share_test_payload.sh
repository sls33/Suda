## bash run_psi_to_share_test_payload.sh > run_psi_to_share_test_payload.log 2>&1 &
sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: htb default 1
sudo tc class add dev lo parent 1: classid 1:1 htb rate 75Mbit
sudo tc qdisc add dev lo parent 1:1 handle 2:0 netem delay 20ms

for host_log_n_data in 20
do
    for batch_size in 1024
    do
        for payload_size in 100 200 500 1000 1500 2000
        do
            echo "--------------------------------------------------------------------"
            echo "host_log_n_data=$host_log_n_data, batch_size=$batch_size, payload_size=$payload_size"
            ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 0 & #> ./log_wan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_0.log 2>&1 &
            ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 1  # > ./log_wan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_1.log 2>&1 
            sleep 3
            echo "--------------------------------------------------------------------"
        done
    done
done


sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: htb default 1
sudo tc class add dev lo parent 1: classid 1:1 htb rate 1Gbps


for host_log_n_data in 20
do
    for batch_size in 1024
    do
        for payload_size in 100 200 500 1000 1500 2000
        do
            echo "--------------------------------------------------------------------"
            echo "host_log_n_data=$host_log_n_data, batch_size=$batch_size, payload_size=$payload_size"
            ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 0 & #> ./log_lan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_0.log 2>&1 &
            ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 1 # > ./log_lan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_1.log 2>&1 
            sleep 3
            echo "--------------------------------------------------------------------"
        done
    done
done


sudo tc qdisc del dev lo root
