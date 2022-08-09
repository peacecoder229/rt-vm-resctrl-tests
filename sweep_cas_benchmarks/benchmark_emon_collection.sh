#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

export PATH=$PATH:/opt/intel/sep_private_5.31_linux_121516364bb91f7/bin64/


ulimit -n 66565
echo 3 > /proc/sys/vm/drop_caches && swapoff -a && swapon -a && printf '\n%s\n' 'Ram-cache and Swap Cleared'
sleep 1

echo "./start-nginx.sh <number of cores> <port>"


umount resctrl
pqos -R
#cores=12
cpupower frequency-set -u 2300000 -d 2300000

pattern=2
cores=2
#for cores in {13..55..14}
#do
#for pattern in {6..12..6}
#do
   for casValue in {255..255..30}
   do
      echo "Starting EMON"
      emon -i spr-events.txt >> mixed_bm_emon.dat &
      
      
      ./nginx-unet-mlc/nginx-mlc-unet.sh
      emon -stop
      
      
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

      mv ../mixed_bm_emon.dat .

      ruby edp.rb -i mixed_bm_emon.dat -m spr-2s.xml -o mixed_bm_emon_out.output --socket-view


      CAS_READ=$(xlsx2csv mixed_bm_emon_out.output.xlsx | grep UNC_M_CAS_COUNT.RD |  cut -d',' -f2)
      CAS_WRITE=$(xlsx2csv  mixed_bm_emon_out.output.xlsx | grep UNC_M_CAS_COUNT.WR |  cut -d',' -f2)
      CAS_ALL=$(xlsx2csv  mixed_bm_emon_out.output.xlsx | grep UNC_M_CAS_COUNT.ALL |  cut -d',' -f2)
     
      READ_BW=$(xlsx2csv mixed_bm_emon_out.output.xlsx | grep "metric_memory bandwidth read" |  cut -d',' -f2)
      WRITE_BW=$(xlsx2csv  mixed_bm_emon_out.output.xlsx | grep "metric_memory bandwidth write" |  cut -d',' -f2)
      TOTAL_BW=$(xlsx2csv  mixed_bm_emon_out.output.xlsx | grep "metric_memory bandwidth total" |  cut -d',' -f2)
     


      cd -

      echo $cores,$casValue,$mlcBwScore,$mlcLaScore,$CAS_READ,$CAS_WRITE,$CAS_ALL,$READ_BW,$WRITE_BW,$TOTAL_BW >> cas_mlc_solo_read_write_log/emon_bench.csv

   #done
#echo $cores
#done
#done 
done
#echo $cores
#done
#'

