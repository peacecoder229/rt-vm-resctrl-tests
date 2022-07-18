#!/bin/bash

source sst_hwdrc_rest.sh
./clean.sh
setup_env
echo "Running Colocation"
./run_3dunet.sh res 32 24 &

./start-nginx.sh 0-15 80

sleep 15
mlc --loaded_latency -W6 -t330 -c16 -k17-23 -d0 &
P0=$!

numactl --membind=1 --physcpubind=56-71 ./wrk/wrk -t 16 -c 880 -d 300s -L http://127.0.0.1:80/1K

wait $P0

cat res

./clean.sh

sleep 20

HPCORE=0-15
LPCORE=16-55

echo "Running with SST"

sleep 2
setup_env
sleep 1

hpfreq="3500"
lpfreq="1200"

sst_config_tf $HPCORE $LPCORE $hpfreq $lpfreq

./run_3dunet.sh res 32 24 &

./start-nginx.sh 0-15 80

sleep 30
mlc --loaded_latency -W6 -t360 -c16 -k17-23 -d0 &
P0=$!

numactl --membind=1 --physcpubind=56-71 ./wrk/wrk -t 16 -c 880 -d 300s -L http://127.0.0.1:80/1K

wait $P0

cat res

./clean.sh

sleep 10

echo "Running with HWDRC"
setup_env

THROTTLE=135

hwdrc_enable $THROTTLE $HPCORE $LPCORE


./run_3dunet.sh res 32 24 &

./start-nginx.sh 0-15 80

sleep 30
mlc --loaded_latency -W6 -t360 -c16 -k17-23 -d0 &
P0=$!

numactl --membind=1 --physcpubind=56-71 ./wrk/wrk -t 16 -c 880 -d 300s -L http://127.0.0.1:80/1K

wait $P0

cat res

./clean.sh

sleep 10

echo "Running HWDRC + SST"



setup_env 

sst_config_tf $HPCORE $LPCORE $hpfreq $lpfreq

hwdrc_enable $THROTTLE $HPCORE $LPCORE


./run_3dunet.sh res 32 24 &

./start-nginx.sh 0-15 80

sleep 30
mlc --loaded_latency -W6 -t360 -c16 -k17-23 -d0 &
P0=$!

numactl --membind=1 --physcpubind=56-71 ./wrk/wrk -t 16 -c 880 -d 300s -L http://127.0.0.1:80/1K

wait $P0

cat res

./clean.sh

