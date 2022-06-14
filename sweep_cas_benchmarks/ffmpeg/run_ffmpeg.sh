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
rm -rf uhd*.mp4

pqos -m "all:[0-55]"  >> monitor_temp.csv &




taskset -c 0-7 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd1.mp4 2> cores_8_ffmpeg.csv &

taskset -c 8-15 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd2.mp4 2> cores_16_ffmpeg.csv & 

taskset -c 16-23 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd3.mp4 2> cores_24_ffmpeg.csv &

taskset -c 24-31 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd4.mp4 2> cores_32_ffmpeg.csv &


taskset -c 32-39 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd5.mp4 2> cores_40_ffmpeg.csv &


taskset -c 40-47 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd6.mp4 2> cores_48_ffmpeg.csv &

taskset -c 48-55 ffmpeg -i uhd1.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd7.mp4 2> cores_55_ffmpeg.csv &



#taskset -c 40-47 ffmpeg -i uhd2.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 200M -maxrate 200M -threads 32 -g 120 -tune psnr -report uhd6.mp4 &
P1=$!

#nice -n -5 taskset -c 16-31 ffmpeg -i uhd2.webm -preset ultrafast -c:v libx264 -b:v 100M -bufsize 100M -maxrate 200M -threads 16 -g 120 -tune psnr -report uhd2.mp4

#mlc --loaded_latency -W6 -t400 -d0 -c0 -k1-15


wait $P1

#sleep 1
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
