#!/bin/bash


mkdir -p /home/dataset/pytorch

cp rnnt.sh /home/dataset/pytorch

cd /home/dataset/pytorch

# Pull the image
curl -u<USERNAME>:<PASSWORD> -O "https://ubit-artifactory-or.intel.com/artifactory/list/dpgpaivsoworkloads-or-local/dlboost/dataset/pytorch/rnnt_V3.tar"

tar xvf rnnt_V3.tar


