#!/bin/bash

echo 3 > /proc/sys/vm/drop_caches && swapoff -a && swapon -a && printf '\n%s\n' 'Ram-cache and Swap Cleared'
sleep 5
HPCORE=0-11
LPCORE=12-47
CAS=32
source /root/colocframework/sst_hwdrc_rest.sh
sleep 1

#for CAS in "16" "32" "48" "64" "72" "80" "88" "96" "104" "112" "120" "128" "136" "148" "164" "180" "196" "225"
#for CAS in "16" "48" "72" "88" "104" "120" "128" "136" "148" "156" "164" "172" "180" "196" "225"
#for CAS in "16" "88" "148" "164" "180" "248"
#for CAS in "16"
#for CAS in "112"
#do

echo "run with SST"

sleep 2
setup_env
sleep 1
hpfreq="3500"
lpfreq="1200"
#sst_config $HPCORE $LPCORE $hpfreq $lpfreq
sst_config_tf $HPCORE $LPCORE $hpfreq $lpfreq
sleep 2

hwdrc_enable $CAS $HPCORE $LPCORE

sleep 2

cp test.csv cas32_ssttf_${hpfreq}_${lpfreq}.csv

#./collect_ngx_mc_ff.py sst_${hpfreq}_${lpfreq}.csv
#./collect_ngx_mc_ff.py cas32_ssttf_${hpfreq}_${lpfreq}.csv
./collect_ngx_mc_ff.py DRC+SST-TF.csv



sleep 2

pkill -9 redis-server
pkill -9 db_bench

#done







