#!/bin/bash


echo "Povray only (0-6)"



/home/cpu2017/run_cpu2017.sh 0-6 povray_r solo_pvary

echo "Running rn50 (28-55)"

docker run --rm --cpuset-cpus=28-55 --name=rn50  -e OMP_NUM_THREADS=28 mxnet_benchmark

echo "Povary + mlc: NO HWDRC"

/home/cpu2017/run_cpu2017.sh 0-6 povray_r povary_mlc &
/home/mlc --loaded_latency -R -t300 -d7 -k8-27

echo "Povary + mlc + rn50: NO HWDRC"





echo "Povary + mlc; rn50: HWDRC"


