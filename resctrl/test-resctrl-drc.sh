# Author: Rohan Tabish
# Description: This files tests the resctl interface
#!/bin/bash
umount resctrl
pqos -R

for i in {56..111}
do
	echo 0 > /sys/devices/system/cpu/cpu$i/online
done


echo "Running RN50 without any association SOLO"
docker run --rm --name=rn50 --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark 
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
sleep 5

echo "==========================Coscheduled RN50 =============================="

docker run --rm --name=rn50_hp --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark &
docker run --rm --name=rn50_lp --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark
#/home/mlc --loaded_latency -R -t200 -d0 &
#/home/mlc --loaded_latency -R -t200 -d0
#docker run --rm --cpuset-mems=0 -e RUNTIME=200 -e MSIZEMB=16384 -e MTHREAD=56 -e CTHREAD=2 stressapp:latest
sleep 120



echo "=================================================Coscheduled RN50 by throttling LP at 90=================="

cd hwdrc_postsi/scripts
./hwdrc_icx_2S_xcc_init_to_default_resctrl.sh
cd -
#CLOS4 & CLOS7

docker run --rm --name=rn50_hpt --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark &
#/home/mlc --loaded_latency -R -t200 -d0 &
sleep 2
P1=$!
echo $P1 > /sys/fs/resctrl/C04/tasks

container_id=$(docker container ls | grep 'rn50_hpt' | awk '{print $1}')
echo $container_id
P1=$(docker top $container_id | grep root | awk '{print $2}')
PP1=$(docker top $container_id | grep root | awk '{print $3}')


for process in $P1
do
	echo $process > /sys/fs/resctrl/C04/tasks
done

for process in $PP1
do 
	echo $process > /sys/fs/resctrl/C04/tasks
done



docker run --rm --name=rn50_lpt --cpu-shares=56 -e OMP_NUM_THREADS=56 mxnet_benchmark &
#/home/mlc --loaded_latency -R -t200 -d0 &
#docker run --rm --cpuset-mems=0 -e RUNTIME=200 -e MSIZEMB=16384 -e MTHREAD=56 -e CTHREAD=2 stressapp:latest &
sleep 2
P0=$!
echo $P0 > /sys/fs/resctrl/C07/tasks



container_id=$(docker container ls | grep 'rn50_lpt' | awk '{print $1}')
echo $container_id
P0=$(docker top $container_id | grep root | awk '{print $2}')
PP0=$(docker top $container_id | grep root | awk '{print $3}')


for process in $P0
do
	echo $process > /sys/fs/resctrl/C07/tasks
done

for process in $PP0
do 
	echo $process > /sys/fs/resctrl/C07/tasks
done


sleep 300
