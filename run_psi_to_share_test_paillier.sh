for host_log_n_data in 20 
do
    for batch_size in 1024
    do
        for payload_size in 8
        do
            echo "--------------------------------------------------------------------"
            echo "host_log_n_data=$host_log_n_data, batch_size=$batch_size, payload_size=$payload_size"
            ./build/bin/psi_to_share_paillier_test $host_log_n_data $batch_size $payload_size 0  &
            ./build/bin/psi_to_share_paillier_test $host_log_n_data $batch_size $payload_size 1 
            sleep 3
            echo "--------------------------------------------------------------------"
        done
    done
done
