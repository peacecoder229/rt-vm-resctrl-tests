#!/bin/bash

killall -9 wrk
rm -rf ins_*
rm -rf results
../clean.sh




for i in {1..24}
do

        ./wrk -t 3 -c 180 -d 60s -L http://192.168.232.254:$((9000+i-1)) > ins_$i &
        #sleep 3
done

sleep 70

for i in {1..24}
do
        cat ins_${i} | grep MB |  awk '{print $2}' | cut -b 1-6 >> results
done

cat results | awk '{ sum += $1; n++ } END { if (n > 0) print sum / 1; }'

