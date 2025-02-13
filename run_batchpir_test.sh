## nohup bash run_batchpir_test.sh >batch_pir.log  2>&1 &
for host_log_n_data in 20 22 24
do
    for batch_size in 1024 2048 4096
    do
        for payload_size in 256 1000
        do
            echo "--------------------------------------------------------------------"
            echo "host_log_n_data=$host_log_n_data, batch_size=$batch_size, payload_size=$payload_size"
            ./build/bin/batch_pir_protocol_test $host_log_n_data $batch_size $payload_size
            echo "--------------------------------------------------------------------"
        done
    done
done


# ./build/bin/batch_pir_protocol_test

# ./build/bin/batch_pir_to_share_test

# ./build/bin/psi_to_share_test