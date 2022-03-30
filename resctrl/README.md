## Simple Resctrl and MBA test

This program tests the resctrl interface with the docker containers



# RESULTS

## MLC Performance 56 Cores No Core association with resctrl (Launched on 56 Cores)

LP Only --> 237786.7

HP only --> 237497.7

LP + HP only no resctrl interface --> 117763.3 + 117719.8

LP + HP with resctrl interface --> 29536.9 + 199884.9

## RN50 + RN50 No core association resctrl hwdrc (Launched on 56 Cores)

LP or HP -> 265.17

LP + HP No HWDRC -> 115.1 + 115.291

LP + HP with resctrl interface ->  66.968+ 114.273563

## RN50 + RN50 No core association resctrl MBA (Launched on 56 cores)

LP or HP -> 265.51

LP + HP No MBA -> 117.641 + 117.15

LP + HP with MBA -> 67.60 + 116.11


in previours DRC validation HP 60% cores vs LP 40% cores
in pqos test: 
rn50 + rn50 

HP	LP
Baseline	149	122.1
HWDRC	167.7	78
hponly-hwdrc	176.6	0
hponly	176	0
MBA10	167.9	39.1

in resctrl test:
rn50 + rn50 

HP	LP
Baseline	148.8	120.6
HWDRC	77.6	61.3
MBA10	165.4	40
MBA20	156.5	103.1
MBA30	152.2	111.8
