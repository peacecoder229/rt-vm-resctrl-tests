icl /c /Od /D INTERNAL /D AVX512_SUPPORT allocation.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT bandwidth.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT busythread.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT combined-loadlat.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT cpuid.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT exit.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT files.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT helper.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT hittest.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT initialize.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT intrd.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT loadlatency.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT main.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT mlcdrv-access.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT mlcdrv-install.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT msr.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT topology.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT topologyHelper.cpp
icl /c /Od /D INTERNAL /D AVX512_SUPPORT traffic.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT winmsr.c
icl /c /Od /D INTERNAL /D AVX512_SUPPORT winlp.c

icl /Femlc_internal.exe allocation.obj bandwidth.obj busythread.obj combined-loadlat.obj cpuid.obj exit.obj files.obj helper.obj hittest.obj initialize.obj intrd.obj loadlatency.obj main.obj mlcdrv-access.obj mlcdrv-install.obj msr.obj topology.obj topologyHelper.obj traffic.obj winmsr.obj winlp.obj advapi32.lib