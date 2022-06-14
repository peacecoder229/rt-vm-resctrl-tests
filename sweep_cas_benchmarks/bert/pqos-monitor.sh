#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

rm -rf *.csv
   
filename=$1

entries=$(grep -irn "CORE" $filename | cut -d : -f 1)
for entry in $entries
do
   line=''"$((entry+1))"'!d'
   echo $line
   sed $line $filename | awk '{print $3}' >> MISSES_monitor.csv
   sed $line $filename | awk '{print $4}' >> LLC_monitor.csv
   sed $line $filename | awk '{print $5}' >> MBL_monitor.csv
done

echo $entries

MISSES=$(cat MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
LLC=$(cat LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
MBL=$(cat MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')

   echo $MBL

