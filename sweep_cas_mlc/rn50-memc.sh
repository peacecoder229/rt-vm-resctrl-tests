#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

umount resctrl
pqos -R

cores=28
offset=$((cores-1))
echo $cores
echo $((cores+offset))

declare -a exe_duration=("385" "180" "140" "130" "128" "128" "128" "128" "128")
count=0
for casValue in {15..255..30}
do
   cd $PWD/hwdrc_postsi/scripts
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$((cores+offset)) 100-101 #$cores-$((cores+offset))
   cd -
   echo "pqos"
   #echo ${exe_duration[casValue]}
  #mlc --loaded_latency -R -t"${exe_duration[$count]}" -d0 -c0 -k1-$((cores-1)) | grep 00000  > mlc_temp &
   #mlc --loaded_latency -R -t120 -d0 -c0 -k1-$((cores-1)) | grep 00000 > mlc_temp &
   #docker run --rm --name=rn50_1 --cpuset-cpus=0-$((cores-1)) -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  rn50_temp_2 &
   /root/memc_redis/mc_rds_in_vm.sh 0-$cores mem_res 2 16 memcache_text 56 /root/memc_redis > memc_data &
   MLCP1=$!
   sleep 80
   pqos -m "all:[$cores-$((cores+offset))]"  >> ${cores}_rn50_1_monitor_temp.csv &
   LPMON=$!
   pqos -m "all:[0-$((cores-1))]" >> ${cores}_rn50_2_monitor_temp.csv &
   HPMON=$!
   count=$((count+1))
   docker run --rm --name=rn50 --cpuset-cpus=$cores-$((cores+offset)) -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  rn50_temp_1
   kill -SIGINT $LPMON
   wait $MLCP1
   killall -SIGINT pqos

   img_per_sec_1=$(grep "images per second" rn50_temp_1 | awk '{print $3}')
   memc_results=$(grep "mcresults" memc_data)


   entries_rn50_1=$(grep -irn "CORE" ${cores}_rn50_1_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_rn50_1
   do
       line=''"$((entry+1))"'!d'
       echo $line
       sed $line ${cores}_rn50_1_monitor_temp.csv | awk '{print $3}' >> ${cores}_rn50_1_MISSES_monitor.csv
       sed $line ${cores}_rn50_1_monitor_temp.csv | awk '{print $4}' >> ${cores}_rn50_1_LLC_monitor.csv
       sed $line ${cores}_rn50_1_monitor_temp.csv | awk '{print $5}' >> ${cores}_rn50_1_MBL_monitor.csv
   done
   echo $entries_mlc

   entries_rn50_2=$(grep -irn "CORE" ${cores}_rn50_2_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_rn50_2
   do
        line=''"$((entry+1))"'!d'
        echo $line
        sed $line ${cores}_rn50_2_monitor_temp.csv | awk '{print $3}' >> ${cores}_rn50_2_MISSES_monitor.csv
        sed $line ${cores}_rn50_2_monitor_temp.csv | awk '{print $4}' >> ${cores}_rn50_2_LLC_monitor.csv
        sed $line ${cores}_rn50_2_monitor_temp.csv | awk '{print $5}' >> ${cores}_rn50_2_MBL_monitor.csv
   done


   MISSES_rn50_1=$(cat ${cores}_rn50_1_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   LLC_rn50_1=$(cat ${cores}_rn50_1_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   MBL_rn50_1=$(cat ${cores}_rn50_1_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')


   MISSES_rn50_2=$(cat ${cores}_rn50_2_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   LLC_rn50_2=$(cat ${cores}_rn50_2_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   MBL_rn50_2=$(cat ${cores}_rn50_2_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')

   rm -rf rn50_temp_1
   rm -rf memc_data

   rm -rf ${cores}_rn50_1_MISSES_monitor.csv
   rm -rf ${cores}_rn50_1_MBL_monitor.csv
   rm -rf ${cores}_rn50_1_monitor_temp.csv
   rm -rf ${cores}_rn50_1_LLC_monitor.csv


   rm -rf ${cores}_rn50_2_MISSES_monitor.csv
   rm -rf ${cores}_rn50_2_MBL_monitor.csv
   rm -rf ${cores}_rn50_2_monitor_temp.csv
   rm -rf ${cores}_rn50_2_LLC_monitor.csv

   echo $cores,$casValue,$img_per_sec_1,$MISSES_rn50_1,$LLC_rn50_1,$MBL_rn50_1 >> ${cores}_rn50_1_monitor.csv
   echo $cores,$casValue,$memc_results,$MISSES_rn50_2,$LLC_rn50_2,$MBL_rn50_2 >> ${cores}_memc_2_monitor.csv

done

