#!/bin/bash

# Author: Rohan Tabish
# Organization: Intel Corporation
# Description: This script monitors the memory BW utilization of the ffmpeg

echo off > /sys/devices/system/cpu/smt/control
cpupower frequency-set -u 2300000 -d 2300000

rm -rf *.log
rm -rf *.csv
rm -rf uhd*.mp4

pqos -m "all:[0-55]"  >> monitor_temp.csv &


taskset -c 16-23 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd1.mp4 &




P1=$!

wait $P1


killall -SIGINT pqos

entries=$(grep -irn "CORE" monitor_temp.csv | cut -d : -f 1)
for entry in $entries
do
   line=''"$((entry+1))"'!d'
   echo $line
   sed $line monitor_temp.csv | awk '{print $3}' >> MISSES_monitor.csv
   sed $line monitor_temp.csv | awk '{print $4}' >> LLC_monitor.csv
   sed $line monitor_temp.csv | awk '{print $5}' >> MBL_monitor.csv
done

MISSES=$(cat MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
LLC=$(cat LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
MBL=$(cat MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')

echo $MISSES,$LLC,$MBL
