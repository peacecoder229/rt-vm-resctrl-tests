#!/bin/bash

echo "Remove previously active containers"

docker container stop $(docker container ls -aq)

docker container rm $(docker container ls -aq)



sleep 10

docker pull dcsorepo.jf.intel.com/dlboost/pytorch:2022_ww16

docker run -itd --privileged --net host --shm-size 4g --name pytorch_spr_2022_ww$1 \
           -v /home/dataset/pytorch:/home/dataset/pytorch \
           -v /home/dl_boost/log/pytorch:/home/dl_boost/log/pytorch \
           dcsorepo.jf.intel.com/dlboost/pytorch:2022_ww16 bash


sleep 10
cp rnnt.sh /home/dataset/pytorch/

total_cores=$1

docker exec pytorch_spr_2022_ww$1 /bin/bash -c "/home/dataset/pytorch/rnnt.sh $total_cores"

cd /home/dataset/pytorch

# process data after run
start=0
total_avg=0.0
for log in *.log; do
  avg=$(grep "Throughput:" $log | awk '{print $2}')
  
  total_avg=$(awk "BEGIN{ print $avg + $total_avg}")

  start=$((start+4))
done

cd -

echo "$total_avg"

