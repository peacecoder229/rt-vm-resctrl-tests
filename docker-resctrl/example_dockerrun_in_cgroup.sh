# Author: Rohan Tabish
# Description: This files tests the resctl interface
#!/bin/bash
umount resctrl
pqos -R

for i in {48..95}
do
	echo 0 > /sys/devices/system/cpu/cpu$i/online
done

#mount -t resctrl resctrl /sys/fs/resctrl
#Note cpu.shares total needs to be >1024. then its shared in ratio of (hp/hp+lp) and lp/hp+lp

echo " Creating cgroups rn50_hpt and rn50_lpt"
./create_cgroup.sh "cpu" "rn50_hpt" "shares:1024"
./create_cgroup.sh "cpu" "rn50_lpt" "shares:512"

sleep 2

echo "MB:0=100" > /sys/fs/resctrl/p0/schemata
echo "MB:0=100" > /sys/fs/resctrl/p1/schemata

#echo "L3:0=3ff\nMB:0=10" > /sys/fs/resctrl/p0/schemata
#echo "


#CLOS4 & CLOS7

docker run --rm --name=rn50_hpt -c 1024 -e OMP_NUM_THREADS=48 mxnet_benchmark &
PH=$!
#/home/mlc --loaded_latency -R -t200 -d0 &
sleep 5

echo $PH > /sys/fs/cgroup/cpu/rn50_hpt/tasks



docker run --rm --name=rn50_lpt -c 512 -e OMP_NUM_THREADS=48 mxnet_benchmark &
PH=$!
sleep 2
echo $PH > /sys/fs/cgroup/cpu/rn50_lpt/tasks
#/home/mlc --loaded_latency -R -t200 -d0 &
#docker run --rm --cpuset-mems=0 -e RUNTIME=200 -e MSIZEMB=16384 -e MTHREAD=56 -e CTHREAD=2 stressapp:latest &
sleep 25

./attach_docker_to_cgrp.sh "rn50_hpt"
./attach_docker_to_cgrp.sh "rn50_lpt"

sleep 2
#cat /sys/fs/cgroup/cpu/rn50_lpt/tasks
for i in {1..10}
do
	sleep 2
	echo "LP tasks"
	cat /sys/fs/cgroup/cpu/rn50_lpt/tasks
done
