# Author: Rohan Tabish
# Description: This files tests the resctl interface
#!/bin/bash
umount resctrl
pqos -R

for i in {56..111}
do
	echo 0 > /sys/devices/system/cpu/cpu$i/online
done

#mount -t resctrl resctrl /sys/fs/resctrl
mount -t resctrl resctrl /sys/fs/resctrl

mkdir /sys/fs/resctrl/p0
mkdir /sys/fs/resctrl/p1

cat /sys/fs/resctrl/p0/schemata
#/home/mlc --loaded_latency -R -t200 -d0


#echo "MB:0=10" > /sys/fs/resctrl/p0/schemata
#cat /sys/fs/resctrl/p0/schemata
#/home/mlc --loaded_latency -R -t200 -d0 &
#docker run --rm --name=rn50 --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark &
#sleep 3

#container_id=$(docker container ls | grep 'rn50' | awk '{print $1}')
#echo $container_id
#P0=$(docker top $container_id | grep root | awk '{print $2}')
#PP0=$(docker top $container_id | grep root | awk '{print $3}')
#PID=$(ps | grep docker | awk '{print $1}')
#PID=$!
#echo $PID  > /sys/fs/resctrl/p0/tasks
#echo $PID
#echo $P0
#echo $PP0



#for process in $P0
#do
#	echo $process > /sys/fs/resctrl/p0/tasks
#done

#for process in $PP0
#do 
#	echo $process > /sys/fs/resctrl/p0/tasks
#done

#cat /sys/fs/resctrl/p0/tasks




#echo $P0 > /sys/fs/resctrl/p0/tasks 

#/home/mlc --loaded_latency -R -t200 -d0



echo "MB:0=100" > /sys/fs/resctrl/p0/schemata
echo "MB:0=100" > /sys/fs/resctrl/p1/schemata

#echo "L3:0=3ff\nMB:0=10" > /sys/fs/resctrl/p0/schemata
#echo "


#CLOS4 & CLOS7

docker run --rm --name=rn50_hpt --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark &
#/home/mlc --loaded_latency -R -t200 -d0 &
sleep 2
P1=$!
echo $P1 > /sys/fs/resctrl/p1/tasks

container_id=$(docker container ls | grep 'rn50_hpt' | awk '{print $1}')
echo $container_id
P1=$(docker top $container_id | grep root | awk '{print $2}')
PP1=$(docker top $container_id | grep root | awk '{print $3}')

for process in $P1
do
	echo $process > /sys/fs/resctrl/p1/tasks
	echo $process > /sys/fs/cgroup/cpu/hpjob/tasks
done

for process in $PP1
do 
	echo $process > /sys/fs/resctrl/p1/tasks
	echo $process > /sys/fs/cgroup/cpu/hpjob/tasks
done


docker run --rm --name=rn50_lpt --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark &
#/home/mlc --loaded_latency -R -t200 -d0 &
#docker run --rm --cpuset-mems=0 -e RUNTIME=200 -e MSIZEMB=16384 -e MTHREAD=56 -e CTHREAD=2 stressapp:latest &
sleep 2
P0=$!
echo $P0 > /sys/fs/resctrl/p0/tasks



container_id=$(docker container ls | grep 'rn50_lpt' | awk '{print $1}')
echo $container_id
P0=$(docker top $container_id | grep root | awk '{print $2}')
PP0=$(docker top $container_id | grep root | awk '{print $3}')


for process in $P0
do
	echo $process > /sys/fs/resctrl/p0/tasks
	echo $process > /sys/fs/cgroup/cpu/lpjob/tasks
done

for process in $PP0
do 
	echo $process > /sys/fs/resctrl/p0/tasks
	echo $process > /sys/fs/cgroup/cpu/lpjob/tasks
done



