#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

   


   entries_rn50_1=$(grep -irn "CORE" $1 | cut -d : -f 1)
   for entry in $entries_rn50_1
   do
       line=''"$((entry+1))"'!d'
       echo $line
       sed $line $1 | awk '{print $3}' >> MISSES_monitor.csv
       sed $line $1 | awk '{print $4}' >> LLC_monitor.csv
       sed $line $1 | awk '{print $5}' >> MBL_monitor.csv
   done
   echo $entries_mlc

   MISSES=$(cat MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   LLC=$(cat LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   MBL=$(cat MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')

rm -rf *.csv

echo "MISSES, LLC, MBL"
echo $MISSES, $LLC, $MBL
