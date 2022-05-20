#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

   


   entries_rn50_1=$(grep -irn "CORE" $1 | cut -d : -f 1)
   for entry in $entries_rn50_1
   do
       line=''"$((entry+1))"'!d'
       echo $line
       sed $line $1 | awk '{print $3}' >> ${cores}_rn50_1_MISSES_monitor.csv
       sed $line $1 | awk '{print $4}' >> ${cores}_rn50_1_LLC_monitor.csv
       sed $line $1 | awk '{print $5}' >> ${cores}_rn50_1_MBL_monitor.csv
   done
   echo $entries_mlc

   MISSES_rn50_1=$(cat $1 | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   LLC_rn50_1=$(cat $1 | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   MBL_rn50_1=$(cat $1 | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')


