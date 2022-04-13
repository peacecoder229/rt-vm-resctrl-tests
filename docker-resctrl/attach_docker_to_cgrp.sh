#!/bin/bash
dockername=$1
#P0=$(ps aux | grep "docker run --rm " | grep -v "color" | awk '{print $2}')
#for p in $P0
#do
#	echo $p > /sys/fs/cgroup/cpu/$dockername/tasks
#done

echo "Initial PID in cgroups $dockername"
cat /sys/fs/cgroup/cpu/$dockername/tasks
sleep 2

container_id=$(docker container ls | grep "$dockername" | awk '{print $1}')

echo "Found  and attaching container ID $container_id"

P0=$(docker top $container_id | grep root | awk '{print $2}')
PP0=$(docker top $container_id | grep root | awk '{print $3}')

echo "PID and PPIDs"
echo $P0
echo $PP0


for p in $P0
do
	echo $p
	echo $p > /sys/fs/cgroup/cpu/$dockername/tasks
done


for p in $PP0
do
	echo $p
	echo $p > /sys/fs/cgroup/cpu/$dockername/tasks
done

echo "End of PID pinning"
cat /sys/fs/cgroup/cpu/$dockername/tasks
