#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

export PATH=$PATH:/opt/intel/sep_private_5.31_linux_121516364bb91f7/bin64/

umount resctrl
pqos -R
#cores=12
cpupower frequency-set -u 2300000 -d 2300000

pattern=2

for cores in {13..55..14}
do
#for pattern in {6..12..6}
#do
   for casValue in {255..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      #./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      ./hwdrc_icx_2S_xcc_disable.sh
      cd -
      echo "Starting EMON"
      emon -i spr-events.txt >> ${cores}_W_${pattern}_${casValue}_emon.dat &
      #mlc --loaded_latency -W6 -t60 -d0 -k1-$cores | tail -1 | awk '{print $2,$3}' > temp_mlc
      mlc --loaded_latency -W$pattern -t60 -c0 -k1-$cores -d0  | tail -1 | awk '{print $2,$3}' > temp_mlc
      #mlc --loaded_latency -W2 -t60 -c0 -k1-$cores -d0 | tail -1 | awk '{print $2,$3}' > temp_mlc
      emon -stop
      
      mlcLaScore=$(cat temp_mlc | awk '{print $1}')
      mlcBwScore=$(cat temp_mlc | awk '{print $2}')
      rm -rf temp_mlc
      
      #disable HWDRC to speed up the processing of emon
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_disable.sh
      cd -
      
      #Process EMON Data

      cd edp

      #rm -rf *.dat
      rm -rf *.csv
      rm -rf *.output

      mv ../${cores}_W_${pattern}_${casValue}_emon.dat .

      ruby edp.rb -i ${cores}_W_${pattern}_${casValue}_emon.dat -m spr-2s.xml -o ${cores}_W_${pattern}_${casValue}_emon_out.output --socket-view


      CAS_READ=$(xlsx2csv ${cores}_W_${pattern}_${casValue}_emon_out.output.xlsx | grep UNC_M_CAS_COUNT.RD |  cut -d',' -f2)
      CAS_WRITE=$(xlsx2csv  ${cores}_W_${pattern}_${casValue}_emon_out.output.xlsx | grep UNC_M_CAS_COUNT.WR |  cut -d',' -f2)
      CAS_ALL=$(xlsx2csv  ${cores}_W_${pattern}_${casValue}_emon_out.output.xlsx | grep UNC_M_CAS_COUNT.ALL |  cut -d',' -f2)
     
      READ_BW=$(xlsx2csv ${cores}_W_${pattern}_${casValue}_emon_out.output.xlsx | grep "metric_memory bandwidth read" |  cut -d',' -f2)
      WRITE_BW=$(xlsx2csv  ${cores}_W_${pattern}_${casValue}_emon_out.output.xlsx | grep "metric_memory bandwidth write" |  cut -d',' -f2)
      TOTAL_BW=$(xlsx2csv  ${cores}_W_${pattern}_${casValue}_emon_out.output.xlsx | grep "metric_memory bandwidth total" |  cut -d',' -f2)
     


      cd -

      echo $cores,$casValue,$mlcBwScore,$mlcLaScore,$CAS_READ,$CAS_WRITE,$CAS_ALL,$READ_BW,$WRITE_BW,$TOTAL_BW >> cas_mlc_solo_read_write_log/${cores}_W_${pattern}__mlc_monitoring.csv

   #done
#echo $cores
done
#done 
done
#echo $cores
#done
#'

