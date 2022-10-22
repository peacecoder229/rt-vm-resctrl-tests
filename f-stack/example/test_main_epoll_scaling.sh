#!/bin/bash

killall -9 helloworld_epoll

sleep 5
ifconfig ens11 down
../dpdk/usertools/dpdk-devbind.py -u 38:00.0
../dpdk/usertools/dpdk-devbind.py -b mlx5_core 38:00.0

sleep 5

#Note: Please adjust the lcoremask parameter in config.ini file if you change number of processes
./helloworld_epoll --conf config.ini --proc-type=primary --proc-id=0  10000 &

sleep 10

for i in {1..2}
do
  port=$((10000+i))
  ./helloworld_epoll --conf config.ini --proc-type=secondary --proc-id=$i  $port &
  echo $port
  sleep 5
done

