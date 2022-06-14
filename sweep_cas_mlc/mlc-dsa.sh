#!/bin/bash


#echo "Running SOLO DSA"

#./dsa_micros/src/dsa_perf_micros -n256 -s4k -j -f -i200000 -k50-52 -w1 -zF,F -o3

#echo "Running SOLO MLC"

#mlc --loaded_latency -W6 -t120 -d0 -k1-31

echo " Co-scheduling MLC and DSA"

mlc --loaded_latency -W6 -t120 -d0 -k1-31 &

./dsa_micros/src/dsa_perf_micros -n256 -s4k -j -f -i2000000 -k50-52 -w1 -zF,F -o3


