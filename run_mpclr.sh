## bash run_mpclr.sh > run_mpclr.log 2>&1 &

sudo tc qdisc del dev lo root
sudo tc qdisc add dev lo root handle 1: htb default 1
sudo tc class add dev lo parent 1: classid 1:1 htb rate 75Mbit
sudo tc qdisc add dev lo parent 1:1 handle 2:0 netem delay 20ms

echo "--------------------------------------------------------"
echo "------------------------CFIX-------------------------------"

python3 ./mpc_lr/launcher.py -f0 CFIX_output_file.cs0 -f1 CFIX_output_file.cs1 --epochs 100
sleep 3
echo "--------------------------------------------------------"
echo "-----------------------SVHN---------------------------------"

python3 ./mpc_lr/launcher.py -f0 SVHN_output_file.cs0 -f1 SVHN_output_file.cs1 --epochs 100
sleep 3


sudo tc qdisc del dev lo root