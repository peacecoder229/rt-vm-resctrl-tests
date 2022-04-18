# Author: Rohan Tabish
# Description: This files tests the resctl interface
#!/bin/bash
umount resctrl
pqos -R

for i in {48..95}
do
	echo 0 > /sys/devices/system/cpu/cpu$i/online
done



sleep 2



docker run --rm --name=rn50_hpt -c 1024 -e OMP_NUM_THREADS=56 mxnet_benchmark &
PH=$!
#/home/mlc --loaded_latency -R -t200 -d0 &
sleep 5

#echo $PH > /sys/fs/cgroup/cpu/rn50_hpt/tasks

#./attach_docker_to_cgrp.sh "rn50_hpt"


docker run --rm --name=rn50_lpt -c 512 -e OMP_NUM_THREADS=56 mxnet_benchmark &
PH=$!
sleep 2
#echo $PH > /sys/fs/cgroup/cpu/rn50_lpt/tasks
#/home/mlc --loaded_latency -R -t200 -d0 &
#docker run --rm --cpuset-mems=0 -e RUNTIME=200 -e MSIZEMB=16384 -e MTHREAD=56 -e CTHREAD=2 stressapp:latest &
#sleep 25

#./attach_docker_to_cgrp.sh "rn50_hpt"
#./attach_docker_to_cgrp.sh "rn50_lpt"

#sleep 2
#cat /sys/fs/cgroup/cpu/rn50_lpt/tasks
