#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

umount resctrl
pqos -R
cores=12

#for cores in {7..55..2}
#do
for pattern in {2..12..1}
do
   for casValue in {235..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      cd -
      echo "pqos"
      pqos -m "all:[0-$cores]"  >> ${cores}_mlc_monitor_temp.csv &
      #mlc --loaded_latency -W6 -t60 -d0 -k1-$cores | tail -1 | awk '{print $2,$3}' > temp_mlc
      mlc --loaded_latency -W$pattern -t60 -c0 -k1-$cores -d0  | tail -1 | awk '{print $2,$3}' > temp_mlc
      #mlc --loaded_latency -W2 -t60 -c0 -k1-$cores -d0 | tail -1 | awk '{print $2,$3}' > temp_mlc
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
      mlcBwScore=$(cat temp_mlc | awk '{print $1}')
      mlcLaScore=$(cat temp_mlc | awk '{print $2}')
      MISSES=$(cat ${cores}_mlc_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      LLC=$(cat ${cores}_mlc_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      MBL=$(cat ${cores}_mlc_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      
      rm -rf ${cores}_mlc_monitor_temp.csv
      rm -rf ${cores}_mlc_MISSES_monitor.csv
      rm -rf ${cores}_mlc_MBL_monitor.csv
      rm -rf ${cores}_mlc_monitor_temp.csv	
      rm -rf ${cores}_mlc_LLC_monitor.csv
      rm -rf temp_mlc

      echo $cores,$casValue,$mlcBwScore,$mlcLaScore,$MISSES,$LLC,$MBL >> ${cores}_W_nutanix_mlc_monitoring.csv

   #done
#echo $cores
done
done 
: << 'END_COMMENT'
for cores in {7..55..6}
do
   for casValue in {15..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      cd -
      echo "pqos"
      pqos -m "all:[0-$cores]"  >> ${cores}_rn50_monitor_temp.csv &
      docker run --rm --name=rn50 --cpuset-cpus=0-$cores -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  temp
      img_per_sec=$(grep "images per second" temp | awk '{print $3}')
      killall -SIGINT pqos
      entries=$(grep -irn "CORE" ${cores}_rn50_monitor_temp.csv | cut -d : -f 1)
      for entry in $entries
      do
	      line=''"$((entry+1))"'!d'
	echo $line
	sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $3}' >> ${cores}_rn50_MISSES_monitor.csv
        sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $4}' >> ${cores}_rn50_LLC_monitor.csv
	sed $line ${cores}_rn50_monitor_temp.csv | awk '{print $5}' >> ${cores}_rn50_MBL_monitor.csv
      done	      
      
      MISSES=$(cat ${cores}_rn50_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      LLC=$(cat ${cores}_rn50_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      MBL=$(cat ${cores}_rn50_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      
      rm -rf ${cores}_rn50_monitor_temp.csv
      rm -rf ${cores}_rn50_MISSES_monitor.csv
      rm -rf ${cores}_rn50_MBL_monitor.csv
      rm -rf ${cores}_rn50_monitor_temp.csv	
      rm -rf ${cores}_rn50_LLC_monitor.csv

      echo $cores,$casValue,$img_per_sec,$MISSES,$LLC,$MBL >> ${cores}_rn50_monitor.csv

   done
echo $cores
done

END_COMMENT


#for cores in {7..55..6}
#do
#   for casValue in {15..255..30}
#   do
#      cd $PWD/hwdrc_postsi/scripts
#      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
#      cd -
#      pqos -i 10 -m "all:[0-$cores]" -t 60 >> ${cores}_rn50_monitor_temp.csv  
#      docker run --rm --name=rn50 -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  temp
#      cat ${core}_rn50_monitor_temp.csv | awk '{print $3,$4,$5}' >> ${cores}_rn50_monitor.csv
#      img_per_sec=$(grep "images per second" temp | awk '{print $3}')
#      echo $cores,$casValue,$img_per_sec >> ${cores}_rn50.csv
#   done
#echo $cores
#done
#'

