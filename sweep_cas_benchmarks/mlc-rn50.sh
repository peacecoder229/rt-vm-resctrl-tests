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
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$((cores-1)) $cores-$((cores+offset))
   cd -
   echo "pqos"
   #echo ${exe_duration[casValue]}
   pqos -m "all:[$cores-$((cores+offset))]"  >> ${cores}_rn50_monitor_temp.csv &
   LPMON=$!
   pqos -m "all:[0-$((cores-1))]" >> ${cores}_mlc_monitor_temp.csv &
   HPMON=$!
   #mlc --loaded_latency -R -t"${exe_duration[$count]}" -d0 -c0 -k1-$((cores-1)) | grep 00000  > mlc_temp &
   mlc --loaded_latency -R -t120 -d0 -c0 -k1-$((cores-1)) | grep 00000 > mlc_temp &
   MLCP1=$!
   docker run --rm --name=rn50 --cpuset-cpus=$cores-$((cores+offset)) -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  rn50_temp  
   kill -SIGINT $LPMON
  
   count=$((count+1))
   
   wait $MLCP1
   killall -SIGINT pqos
   

   mlcLat=$(cat mlc_temp | awk '{print $2}')
   mlcBW=$(cat mlc_temp | awk '{print $3}')
   img_per_sec=$(grep "images per second" rn50_temp | awk '{print $3}')


   entries_mlc=$(grep -irn "CORE" ${cores}_mlc_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_mlc
   do
       line=''"$((entry+1))"'!d'
       echo $line
       sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $3}' >> ${cores}_mlc_MISSES_monitor.csv
       sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $4}' >> ${cores}_mlc_LLC_monitor.csv
       sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $5}' >> ${cores}_mlc_MBL_monitor.csv
   done	      
   echo $entries_mlc

   entries_rn50=$(grep -irn "CORE" ${cores}_rn50_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_rn50
   do
	line=''"$((entry+1))"'!d'
	echo $line
	sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $3}' >> ${cores}_rn50_MISSES_monitor.csv
        sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $4}' >> ${cores}_rn50_LLC_monitor.csv
	sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $5}' >> ${cores}_rn50_MBL_monitor.csv
   done	      


   MISSES_mlc=$(cat ${cores}_mlc_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_mlc=$(cat ${cores}_mlc_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_mlc=$(cat ${cores}_mlc_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 


   MISSES_rn50=$(cat ${cores}_rn50_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_rn50=$(cat ${cores}_rn50_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_rn50=$(cat ${cores}_rn50_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 

   rm -rf mlc_temp
   rm -rf rn50_temp

   rm -rf ${cores}_mlc_MISSES_monitor.csv
   rm -rf ${cores}_mlc_MBL_monitor.csv
   rm -rf ${cores}_mlc_monitor_temp.csv
   rm -rf ${cores}_mlc_LLC_monitor.csv
   
   
   rm -rf ${cores}_rn50_MISSES_monitor.csv
   rm -rf ${cores}_rn50_MBL_monitor.csv
   rm -rf ${cores}_rn50_monitor_temp.csv
   rm -rf ${cores}_rn50_LLC_monitor.csv

   echo $cores,$casValue,$mlcLat,$mlcBW,$MISSES_mlc,$LLC_mlc,$MBL_mlc >> ${cores}_mlc_monitor.csv
   echo $cores,$casValue,$img_per_sec,$MISSES_rn50,$LLC_rn50,$MBL_rn50 >> ${cores}_rn50_monitor.csv

done


