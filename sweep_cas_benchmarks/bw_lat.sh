#!/bin/bash

#for c in {1..55..6}
#do 
c=56
for casValue in {15..255..30}
do
   cd $PWD/hwdrc_postsi/scripts
   ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $casValue 0-$c 56-57
   cd -
   j=$(( c+1 ))    
   mlcScore=$(mlc --loaded_latency -R -t60 -c${j} -k1-${c} -d0 | grep 00000)
   echo $mlcScore $casValue $c >> bw_latency
done
#done

