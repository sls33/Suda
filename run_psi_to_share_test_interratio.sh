## bash run_psi_to_share_test_interratio.sh > run_psi_to_share_test_interratio.log 2>&1 &
sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: htb default 1
sudo tc class add dev lo parent 1: classid 1:1 htb rate 1Gbps


for host_log_n_data in 20
do
    for batch_size in 4096
    do
        for payload_size in 1000
        do
            for inter_ratio in  0.25  0.5 1.0
            do
                echo "---------------------------------LAN-----------------------------------"
                echo "host_log_n_data=$host_log_n_data, batch_size=$batch_size, payload_size=$payload_size, inter_ratio=$inter_ratio"
                ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 0 $inter_ratio & #> ./log_lan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_${inter_ratio}_0.log 2>&1 &
                ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 1 $inter_ratio   #> ./log_lan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_${inter_ratio}_1.log 2>&1 
                sleep 3
                echo "--------------------------------------------------------------------"
            done
        done
    done
done

sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: htb default 1
sudo tc class add dev lo parent 1: classid 1:1 htb rate 75Mbit
sudo tc qdisc add dev lo parent 1:1 handle 2:0 netem delay 20ms



for host_log_n_data in 20
do
    for batch_size in 4096
    do
        for payload_size in 1000
        do
            for inter_ratio in 0.25 0.5 1.0
            do
                echo "----------------------------------WAN----------------------------------"
                echo "host_log_n_data=$host_log_n_data, batch_size=$batch_size, payload_size=$payload_size"
                ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 0 $inter_ratio & #> ./log_lan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_${inter_ratio}_0.log 2>&1 &
                ./build/bin/psi_to_share_test $host_log_n_data $batch_size $payload_size 1 $inter_ratio  # > ./log_lan/psi_to_share_test_${host_log_n_data}_${batch_size}_${payload_size}_${inter_ratio}_1.log 2>&1 
                sleep 3
                echo "--------------------------------------------------------------------"
            done
        done
    done
done


sudo tc qdisc del dev lo root

# nohup ./build/bin/psi_to_share_test 20 4096 4 0 > psi_to_share_test0.log 2>&1 &
# nohup ./build/bin/psi_to_share_test 20 4096 4 1 > psi_to_share_test1.log 2>&1 