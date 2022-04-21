#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

umount resctrl
pqos -R
: << 'END_COMMENT'
for cores in {7..55..6}
do
   for casValue in {15..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      cd -
      echo "pqos"
      pqos -m "all:[0-$cores]"  >> ${cores}_mlc_monitor_temp.csv &
      mlcScore=$(mlc --loaded_latency -R -t60 -d0 -k1-$cores | tail -1 | awk '{print $3}')
      killall -SIGINT pqos
      entries=$(grep -irn "CORE" ${cores}_mlc_monitor_temp.csv | cut -d : -f 1)
      for entry in $entries
      do
	      line=''"$((entry+1))"'!d'
	echo $line
	sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $3}' >> ${cores}_mlc_MISSES_monitor.csv
        sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $4}' >> ${cores}_mlc_LLC_monitor.csv
	sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $5}' >> ${cores}_mlc_MBL_monitor.csv
      done	      
      
      MISSES=$(cat ${cores}_mlc_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      LLC=$(cat ${cores}_mlc_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      MBL=$(cat ${cores}_mlc_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      
      rm -rf ${cores}_mlc_monitor_temp.csv
      rm -rf ${cores}_mlc_MISSES_monitor.csv
      rm -rf ${cores}_mlc_MBL_monitor.csv
      rm -rf ${cores}_mlc_monitor_temp.csv	
      rm -rf ${cores}_mlc_LLC_monitor.csv

      echo $cores,$casValue,$mlcScore,$MISSES,$LLC,$MBL >> ${cores}_mlc_monitor.csv

   done
echGMW-DCG-0604o $cores
done
END_COMMENT

cores=14
offset=13
echo $cores
echo $((cores+offset))
for casValue in {15..105..30}
do
   cd $PWD/hwdrc_postsi/scripts
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$((cores+offset)) 54-55 #$((cores-1))
   cd -
   echo "pqos"
   pqos -m "all:[$cores-$((cores+offset))]"  >> ${cores}_rn50_monitor_temp.csv &
   LPMON=$!
   pqos -m "all:[0-$((cores-1))]" >> ${cores}_mlc_monitor_temp.csv &
   HPMON=$!
   mlc --loaded_latency -R -t550 -d0 -k1-$((cores-1)) | tail -1 | awk '{print $3}' > mlc_temp &
   MLCP1=$!
   docker run --rm --name=rn50 --cpuset-cpus=$cores-$((cores+offset)) -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  temp
  
   kill -SIGINT $LPMON
  
   #DOCKERP1=$!
   wait $MLCP1
   #echo "Stop LP"
   killall -SIGINT pqos
   #wait $DOCKERP1
   #echo "Stop HP"
   #killall -SIGINT $HPMON
   #filter_mlc=$(wc -l ${cores}_mlc_monitor_temp.csv | awk '{print $1}')
   #cat ${cores}_mlc_monitor_temp.csv | head -n 1600 > ${cores}_mlc_monitor_temp.csv
   

   img_per_sec=$(grep "images per second" temp | awk '{print $3}')
   mlcScore=$(cat mlc_temp)
   entries_rn50=$(grep -irn "CORE" ${cores}_rn50_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_rn50
   do
       line=''"$((entry+1))"'!d'
       echo $line
       sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $3}' >> ${cores}_rn50_MISSES_monitor.csv
       sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $4}' >> ${cores}_rn50_LLC_monitor.csv
       sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $5}' >> ${cores}_rn50_MBL_monitor.csv
   done	      
   echo $entries_rn50

   entries_mlc=$(grep -irn "CORE" ${cores}_mlc_monitor_temp.csv | cut -d : -f 1)
   for entry in $entries_mlc
   do
	line=''"$((entry+1))"'!d'
	echo $line
	sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $3}' >> ${cores}_mlc_MISSES_monitor.csv
        sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $4}' >> ${cores}_mlc_LLC_monitor.csv
	sed $line ${cores}_mlc_monitor_temp.csv | awk '{print $5}' >> ${cores}_mlc_MBL_monitor.csv
   done	      



   MISSES_rn50=$(cat ${cores}_rn50_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_rn50=$(cat ${cores}_rn50_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_rn50=$(cat ${cores}_rn50_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 


   MISSES_mlc=$(cat ${cores}_mlc_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   LLC_mlc=$(cat ${cores}_mlc_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
   MBL_mlc=$(cat ${cores}_mlc_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 

   rm -rf mlc_temp
   rm -rf temp
   rm -rf ${cores}_mlc_MISSES_monitor.csv
   rm -rf ${cores}_mlc_MBL_monitor.csv
   rm -rf ${cores}_mlc_monitor_temp.csv
   rm -rf ${cores}_mlc_LLC_monitor.csv
   

   
   rm -rf ${cores}_rn50_MISSES_monitor.csv
   rm -rf ${cores}_rn50_MBL_monitor.csv
   rm -rf ${cores}_rn50_monitor_temp.csv	
   rm -rf ${cores}_rn50_monitor_temp_test.csv
   rm -rf ${cores}_rn50_LLC_monitor.csv

   echo $cores,$casValue,$img_per_sec,$MISSES_rn50,$LLC_rn50,$MBL_rn50 >> ${cores}_rn50_monitor.csv
   echo $cores,$casValue,$mlcScore,$MISSES_mlc,$LLC_mlc,$MBL_mlc >> ${cores}_mlc_monitor.csv

done


