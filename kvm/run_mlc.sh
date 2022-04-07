mlc --loaded_latency -R -t60 -d0 -k1-$[$(getconf _NPROCESSORS_ONLN)-1] > $1
