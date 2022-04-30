#!/bin/bash

echo "Nginx Coscheduled MLC"


cpupower frequency-set -u 2300000 -d 2300000
echo off > /sys/devices/system/cpu/smt/control

for casValue in {15..255..30}
do
   cd $PWD/hwdrc_postsi/scripts
   #./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-43 44-55 #28-43 #$cores-$((cores+offset))
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-27 28-43 #44-55 #28-43 #$cores-$((cores+offset))
  
   cd -

   sleep 1
   echo "================================================================="
   echo $casValue
   echo "================================================================="


   ./start-nginx.sh 28-43 80
   P0=$!
   wait $P0



   mlc --loaded_latency -W6 -t320 -d0 -k1-27 &
   P1=$!
    

   sleep 1
   numactl --membind=1 --physcpubind=56-71 ./wrk/wrk -t 16 -c 880 -d 300s -L http://127.0.0.1:80/1K
   #numactl --membind=1 --physcpubind=56-71 ./wrk2/wrk -t 32 -c 2048 -d 180s -R 1024000 -L http://127.0.0.1:80/1K
   wait $P1
done

