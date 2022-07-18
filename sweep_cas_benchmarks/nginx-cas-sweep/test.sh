#!/bin/bash

./clean.sh



./start-nginx.sh 28-43 80

sleep 4
mlc --loaded_latency -W6 -t360 -c0 -k1-27 -d0 &
numactl --membind=1 --physcpubind=56-71 ./wrk/wrk -t 16 -c 880 -d 300s -L http://127.0.0.1:80/1K

P0=$!
wait $P0


./clean.sh
