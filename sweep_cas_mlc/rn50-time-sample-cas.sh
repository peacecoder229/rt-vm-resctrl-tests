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
for casValue in {15..255..30}
do
   cd $PWD/hwdrc_postsi/scripts
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue $cores-$((cores+offset)) 56-57
   cd -
   echo "pqos"
   pqos -m "all:[$cores-$((cores+offset))]"  >> ${cores}_rn50_monitor_temp.csv &
   LPMON=$!
   start=$(date +%s.%N)
   docker run --rm --name=rn50 --cpuset-cpus=$cores-$((cores+offset)) -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  rn50_temp  
   end=$(date +%s.%N)
   kill -SIGINT $LPMON
   
   img_per_sec=$(grep "images per second" rn50_temp | awk '{print $3}')

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


   MISSES_rn50=$(cat ${cores}_rn50_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_rn50=$(cat ${cores}_rn50_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_rn50=$(cat ${cores}_rn50_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 

   rm -rf rn50_temp   
   
   rm -rf ${cores}_rn50_MISSES_monitor.csv
   rm -rf ${cores}_rn50_MBL_monitor.csv
   rm -rf ${cores}_rn50_monitor_temp.csv
   rm -rf ${cores}_rn50_LLC_monitor.csv

   echo $cores,$casValue,$img_per_sec,$MISSES_rn50,$LLC_rn50,$MBL_rn50,$start,$end >> ${cores}_rn50_monitor.csv

done


