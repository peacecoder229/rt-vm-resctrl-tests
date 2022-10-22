#!/bin/bash

killall -9 qperf
rm -rf rdma_statistics

qperf &

time=$1

ssh root@10.242.51.105 "qperf -t $time -cm1 192.168.232.254 rc_bw rc_lat" >> rdma_statistics


