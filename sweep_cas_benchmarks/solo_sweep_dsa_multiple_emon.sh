#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file runs DSA test and captures BW

export PATH=$PATH:/opt/intel/sep_private_5.31_linux_121516364bb91f7/bin64/

umount resctrl
pqos -R
#cores=12
cpupower frequency-set -u 2300000 -d 2300000
./dsa_micros/scripts/setup_dsa.sh -d dsa0 -w 1 -m d -e 4
./dsa_micros/scripts/setup_dsa.sh -d dsa2 -w 1 -m d -e 4
./dsa_micros/scripts/setup_dsa.sh -d dsa4 -w 1 -m d -e 4
./dsa_micros/scripts/setup_dsa.sh -d dsa6 -w 1 -m d -e 4



      echo "Starting EMON"
      
      #Start EMON
      emon -i spr-events.txt >> dsa_emon.dat &

      sleep 1

      dsa_output=$(./dsa_micros/src/dsa_perf_micros -n128 -s16K -o3 -j -c -f -k0-3 -w0 -o3 -zF,F -i1000000)           
      #dsa_output=$(./dsa_micros/src/dsa_perf_micros -n128 -s16k -j -c -f -i1000000 -k5 -w0 -zF,F -o3)      
      

      emon -stop
      
      
      
      #Process EMON Data
 
      cd edp

      rm -rf *.dat
      rm -rf *.csv
      rm -rf *.output

      mv ../dsa_emon.dat .

      ruby edp.rb -i dsa_emon.dat -m spr-2s.xml -o dsa_multiple_emon_out.output --socket-view


      CAS_READ=$(grep -ir UNC_M_CAS_COUNT.RD dsa_multiple_emon_out.output.xlsx | cut -d',' -f3) 
      CAS_WRITE=$(grep -ir UNC_M_CAS_COUNT.WR  dsa_multiple_emon_out.output.xlsx | cut -d',' -f3) 
      CAS_ALL=$(grep -ir UNC_M_CAS_COUNT.ALL  dsa_multiple_emon_out.output.xlsx | cut -d',' -f3) 

      READ_BW=$(xlsx2csv  dsa_multiple_emon_out.output.xlsx | grep "metric_memory bandwidth read" |  cut -d',' -f2)
      WRITE_BW=$(xlsx2csv   dsa_multiple_emon_out.output.xlsx | grep "metric_memory bandwidth write" |  cut -d',' -f2)
      TOTAL_BW=$(xlsx2csv   dsa_multiple_emon_out.output.xlsx | grep "metric_memory bandwidth total" |  cut -d',' -f2)



      cd -

      echo $dsa_output,$CAS_READ,$CAS_WRITE,$CAS_ALL,$READ_BW,$WRITE_BW,$TOTAL_BW  >> output_multiple


