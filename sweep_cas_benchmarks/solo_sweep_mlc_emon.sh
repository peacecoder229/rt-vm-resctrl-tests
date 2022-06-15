#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

export PATH=$PATH:/opt/intel/sep_private_5.31_linux_121516364bb91f7/bin64/

umount resctrl
pqos -R
#cores=12

for cores in {7..55..2}
do
for pattern in {2..12..15}
do
   for casValue in {15..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      cd -
      echo "Starting EMON"
      emon -i spr-events.txt >> ${cores}_W_${pattern}_emon.dat &
      #mlc --loaded_latency -W6 -t60 -d0 -k1-$cores | tail -1 | awk '{print $2,$3}' > temp_mlc
      mlc --loaded_latency -W$pattern -t60 -c0 -k1-$cores -d0  | tail -1 | awk '{print $2,$3}' > temp_mlc
      #mlc --loaded_latency -W2 -t60 -c0 -k1-$cores -d0 | tail -1 | awk '{print $2,$3}' > temp_mlc
      emon -stop
      
      mlcBwScore=$(cat temp_mlc | awk '{print $1}')
      mlcLaScore=$(cat temp_mlc | awk '{print $2}')
      rm -rf temp_mlc
      #Process EMON Data
 
      cd edp

      rm -rf *.dat
      rm -rf *.csv
      rm -rf *.output

      mv ../${cores}_W_${pattern}_emon.dat .

      ruby edp.rb -i ${cores}_W_${pattern}_emon.dat -m spr-2s.xml -o ${cores}_W_${pattern}_emon_out.output --socket-view


      CAS_READ=$(grep -ir UNC_M_CAS_COUNT.RD __edp_socket_view_summary.csv | cut -d',' -f3) 
      CAS_WRITE=$(grep -ir UNC_M_CAS_COUNT.WR __edp_socket_view_summary.csv | cut -d',' -f3) 
      CAS_ALL=$(grep -ir UNC_M_CAS_COUNT.ALL __edp_socket_view_summary.csv | cut -d',' -f3) 


      cd -

      echo $cores,$casValue,$mlcBwScore,$mlcLaScore,$CAS_READ,$CAS_WRITE,$CAS_ALL >> ${cores}_W_${pattern}_mlc_monitoring.csv

   #done
#echo $cores
done
done 
done
#echo $cores
#done
#'

