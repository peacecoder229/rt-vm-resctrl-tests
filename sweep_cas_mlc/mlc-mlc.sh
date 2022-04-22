#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

umount resctrl
pqos -R

cores=14
offset=$((cores-1))
echo $cores
echo $((cores+offset))
for casValue in {15..30..30}
do
   #cd $PWD/hwdrc_postsi/scripts
   #./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$((cores+offset)) 56-57 #$cores-$((cores+offset))
   #cd -
   echo "pqos"
   pqos -m "all:[$cores-$((cores+offset))]"  >> ${cores}_mlc_2_monitor_temp.csv &
   LPMON=$!
   pqos -m "all:[0-$((cores-1))]" >> ${cores}_mlc_1_monitor_temp.csv &
   HPMON=$!
   mlc --loaded_latency -R -t300 -d0 -c0 -k1-$((cores-1)) | grep 00000  > mlc_temp1 &
   MLCP1=$!
   mlc --loaded_latency -R -t300 -d0 -c$cores -k$((cores+1))-$((cores+offset)) | grep 00000 > mlc_temp2
  
   kill -SIGINT $LPMON
  
   
   wait $MLCP1
   killall -SIGINT pqos
   

   mlcLat1=$(cat mlc_temp1 | awk '{print $2}')
   mlcLat2=$(cat mlc_temp2 | awk '{print $2}')

   mlcBW1=$(cat mlc_temp1 | awk '{print $3}') 
   mlcBW2=$(cat mlc_temp2 | awk '{print $3}')

   entries_mlc1=$(grep -irn "CORE" ${cores}_mlc_1_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_mlc1
   do
       line=''"$((entry+1))"'!d'
       echo $line
       sed $line ${cores}_mlc_1_monitor_temp.csv | awk '{print $3}' >> ${cores}_mlc_1_MISSES_monitor.csv
       sed $line ${cores}_mlc_1_monitor_temp.csv | awk '{print $4}' >> ${cores}_mlc_1_LLC_monitor.csv
       sed $line ${cores}_mlc_1_monitor_temp.csv | awk '{print $5}' >> ${cores}_mlc_1_MBL_monitor.csv
   done	      
   echo $entries_mlc1

   entries_mlc2=$(grep -irn "CORE" ${cores}_mlc_2_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_mlc2
   do
	line=''"$((entry+1))"'!d'
	echo $line
	sed $line ${cores}_mlc_2_monitor_temp.csv | awk '{print $3}' >> ${cores}_mlc_2_MISSES_monitor.csv
        sed $line ${cores}_mlc_2_monitor_temp.csv | awk '{print $4}' >> ${cores}_mlc_2_LLC_monitor.csv
	sed $line ${cores}_mlc_2_monitor_temp.csv | awk '{print $5}' >> ${cores}_mlc_2_MBL_monitor.csv
   done	      



   MISSES_mlc_1=$(cat ${cores}_mlc_1_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_mlc_1=$(cat ${cores}_mlc_1_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_mlc_1=$(cat ${cores}_mlc_1_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 


   MISSES_mlc_2=$(cat ${cores}_mlc_2_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_mlc_2=$(cat ${cores}_mlc_2_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_mlc_2=$(cat ${cores}_mlc_2_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 

   rm -rf mlc_temp1
   rm -rf mlc_temp2

   rm -rf ${cores}_mlc_1_MISSES_monitor.csv
   rm -rf ${cores}_mlc_1_MBL_monitor.csv
   rm -rf ${cores}_mlc_1_monitor_temp.csv
   rm -rf ${cores}_mlc_1_LLC_monitor.csv
   

   
   rm -rf ${cores}_mlc_2_MISSES_monitor.csv
   rm -rf ${cores}_mlc_2_MBL_monitor.csv
   rm -rf ${cores}_mlc_2_monitor_temp.csv
   rm -rf ${cores}_mlc_2_LLC_monitor.csv

   echo $cores,$casValue,$mlcLat1,$mlcBW1,$MISSES_mlc_1,$LLC_mlc_1,$MBL_mlc_1 >> ${cores}_mlc_1_monitor.csv
   echo $cores,$casValue,$mlcLat2,$mlcBW2,$MISSES_mlc_2,$LLC_mlc_2,$MBL_mlc_2 >> ${cores}_mlc_2_monitor.csv

done


