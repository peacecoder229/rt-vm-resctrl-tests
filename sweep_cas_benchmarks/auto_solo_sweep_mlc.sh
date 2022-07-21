#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This code sweeps through the different throttling values. It picks the throttling value that protects system from entering saturation.
#Email: rohan.tabish@intel.com

umount resctrl
pqos -R
cores=35
#for cores in {5..55..2}
#do
iteration=1
premlcBwScore=0.00
premlcLaScore=0.00
preMBL=0
LaThreshold=40.00

for pattern in {2..12..1}
do
   for casValue in {130..225..2}
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

      mlcLaScore=$(cat temp_mlc | awk '{print $1}')
      mlcBWScore=$(cat temp_mlc | awk '{print $2}')

      MISSES=$(cat ${cores}_mlc_MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      LLC=$(cat ${cores}_mlc_LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      MBL=$(cat ${cores}_mlc_MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }') 
      
      rm -rf ${cores}_mlc_monitor_temp.csv
      rm -rf ${cores}_mlc_MISSES_monitor.csv
      rm -rf ${cores}_mlc_MBL_monitor.csv
      rm -rf ${cores}_mlc_monitor_temp.csv	
      rm -rf ${cores}_mlc_LLC_monitor.csv
      rm -rf temp_mlc

      #"$(echo "${val} < ${min}" | bc)"

      if [ $iteration -gt 1 ]
      then
	      echo "**************************"
	      echo $iteration,$mlcLaScore,$premlcLaScore
	      diff=$(bc <<< "$mlcLaScore-$premlcLaScore")
	      echo $diff,$LaThreshold,$iteration
	      if [ 1 -eq "$(echo "${diff} > ${LaThreshold}" | bc)" ]
	      #if [ $diff -gt $LaThreshold ]
	      then
		      echo $cores,$pattern,$precasValue,$preMBL,$premlcLaScore,$premlcBwScore >> pattern_cas_lat_bw.csv
  	      fi
      fi

      precasValue=$casValue
      premlcLaScore=$mlcLaScore
      premlcBwScore=$mlcBwScore
      preMBL=$MBL
      iteration=$((iteration + 1))
      echo $cores,$pattern,$casValue,$MBL,$mlcLaScore,$mlcBwScore,$MISSES,$LLC,$MBL >> ${cores}_W_${pattern}_nutanix_mlc_monitoring.csv
      
   done
#echo $cores
done
#done 





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

