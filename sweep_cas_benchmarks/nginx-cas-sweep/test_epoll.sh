#!/bin/bash

killall -9 wrk

../clean.sh

sleep 5

for i in {1..3}
do

        ./wrk -t 4 -c 800 -d 10s -L http://192.168.232.254:$((10000+i-1)) &
        sleep 3
done

