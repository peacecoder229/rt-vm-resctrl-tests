#!/bin/bash
#Author: Rohan Tabish
#Organization: Intel Corporation
#Description: This file sweeps through the number of cores and cas value for mlc and finds the bandwidth

:'
for cores in {7..55..6}
do
   for casValue in {15..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      cd -
      mlcScore=$(mlc --loaded_latency -R -t60 -d0 -k1-$cores | tail -1 | awk '{print $3}')
      echo $cores,$casValue,$mlcScore >> ${cores}_mlc.csv
   done
echo $cores
done
'


for cores in {7..55..6}
do
   for casValue in {15..255..30}
   do
      cd $PWD/hwdrc_postsi/scripts
      ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$cores 56-57
      cd -
      docker run --rm --name=rn50 -e OMP_NUM_THREADS=$cores mxnet_benchmark 2>  temp

      img_per_sec=$(grep "images per second" temp | awk '{print $3}')
      echo $cores,$casValue,$img_per_sec >> ${cores}_rn50.csv
   done
echo $cores
done


