#!/bin/bash

# Author: Rohan Tabish
# Organization: University of Illinois at Urbana-Champaign (UIUC)
# Description: This script monitors the memory BW utilization of the ffmpeg

#for threads in {8..28..8}
#do
    #taskset -c 0-$threads ffmpeg -i $1 -c:v libx264 -b:v 3M -bufsize 6M -maxrate 4.5M -threads $threads -g 120 -tune psnr -report sample_$threads.mp4 >> output_$threads
    
#done
echo off > /sys/devices/system/cpu/smt/control
cpupower frequency-set -u 2300000 -d 2300000

rm -rf *.log
rm -rf *.csv
rm -rf sample_28.mp4

pqos -m "all:[0-32]"  >> monitor_temp.csv &

#taskset -c 0-35 ffmpeg -i 1080p_video.mp4 -preset veryslow -c:v libx264 -b:v 50M -bufsize 20M -maxrate 10.5M -threads 36 -g 120 -tune psnr -report sample_28.mp4 


#taskset -c 0-35 ffmpeg -i uhd.webm -c:v libx264 -threads 36 -g 120 -tune psnr -report sample_uhd.mp4 
taskset -c 0-15 ffmpeg -i uhd1.webm -crf 0 -c:v libx264 -report vid-report.mp4 &
taskset -c 16-31 ffmpeg -i uhd2.webm -crf 0 -c:v libx264 -report sample-16.mp4

sleep 1
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

echo $MISSES,$MBL,$LLC
