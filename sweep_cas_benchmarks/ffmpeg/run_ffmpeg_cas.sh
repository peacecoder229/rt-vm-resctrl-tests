#!/bin/bash

# Author: Rohan Tabish
# Organization: Intel Corporation
# Description: This script monitors the memory BW utilization of the ffmpeg

echo off > /sys/devices/system/cpu/smt/control
cpupower frequency-set -u 2300000 -d 2300000


for casValue in {15..255..30}
do
   cd ../hwdrc_postsi/scripts
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-55 56-57 #$cores-$((cores+offset))
   cd -

   rm -rf *.log
   rm -rf *.csv
   rm -rf *.mp4

   pqos -m "all:[0-31]"  >> monitor_temp.csv &


   taskset -c 0-7 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd1.mp4 &

   taskset -c 8-15 ffmpeg -i uhd2.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd2.mp4 & 

   taskset -c 16-23 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd3.mp4 &

   taskset -c 24-31 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd4.mp4 &

   #taskset -c 32-39 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd5.mp4 &

   #taskset -c 40-47 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd6.mp4 &

   #taskset -c 48-55 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd7.mp4 &

   P1=$!

   wait $P1

   killall -SIGINT pqos

   echo "Starting Processing ...."
   entries=$(grep -irn "CORE" monitor_temp.csv | cut -d : -f 1)
   for entry in $entries
   do
      line=''"$((entry+1))"'!d'
      echo $line
      sed $line monitor_temp.csv | awk '{print $3}' >> MISSES_monitor.csv
      sed $line monitor_temp.csv | awk '{print $4}' >> LLC_monitor.csv
      sed $line monitor_temp.csv | awk '{print $5}' >> MBL_monitor.csv
   done

   echo "DONE Processing ...."

   MISSES=$(cat MISSES_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   LLC=$(cat LLC_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')
   MBL=$(cat MBL_monitor.csv | awk '{ sum += $1; n++ } END { if (n > 0) print sum / n; }')

   echo $casValue,$MISSES,$LLC,$MBL >> ffmpeg_sweep_48
done
