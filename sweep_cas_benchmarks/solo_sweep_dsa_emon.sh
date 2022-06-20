#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file runs DSA test and captures BW using the CAS

export PATH=$PATH:/opt/intel/sep_private_5.31_linux_121516364bb91f7/bin64/

umount resctrl
pqos -R
#cores=12
cpupower frequency-set -u 2300000 -d 2300000


      echo "Starting EMON"
      
      #Start EMON
      emon -i spr-events.txt >> dsa_emon.dat &

      sleep 1

      dsa_output=$(./dsa_micros/src/dsa_perf_micros -n128 -s16k -j -c -f -i1000000 -k5 -w0 -zF,F -o3)           
      
      

      emon -stop
      
      
      
      #Process EMON Data
 
      cd edp

      rm -rf *.dat
      rm -rf *.csv
      rm -rf *.output

      mv ../dsa_emon.dat .

      ruby edp.rb -i dsa_emon.dat -m spr-2s.xml -o ${cores}_W_${pattern}_emon_out.output --socket-view


      CAS_READ=$(grep -ir UNC_M_CAS_COUNT.RD __edp_socket_view_summary.csv | cut -d',' -f3) 
      CAS_WRITE=$(grep -ir UNC_M_CAS_COUNT.WR __edp_socket_view_summary.csv | cut -d',' -f3) 
      CAS_ALL=$(grep -ir UNC_M_CAS_COUNT.ALL __edp_socket_view_summary.csv | cut -d',' -f3) 


      cd -

      echo $dsa_output,$CAS_READ,$CAS_WRITE,$CAS_ALL 


