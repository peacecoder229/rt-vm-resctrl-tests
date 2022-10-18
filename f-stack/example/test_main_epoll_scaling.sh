#!/bin/bash

#Note: Please adjust the lcoremask parameter in config.ini file if you change number of processes
./helloworld_epoll --conf config.ini --proc-type=primary --proc-id=0  9000 &

sleep 5
I
for i in {1..1}
do
  port=$((9000+i))
  ./helloworld_epoll --conf config.ini --proc-type=secondary --proc-id=$i  $port &
  echo $port
done

