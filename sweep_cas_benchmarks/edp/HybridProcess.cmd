@echo off
:: Please collect emon data with -v command to avoid errors in post processing
:: ruby interpreter, change it according to the path where ruby is installed in your system
set RUBY="ruby.exe"

:: input file name, you may need to change them
set EMON_DATA=emon.dat
:: ************** EMON -v and -M files need not be collected separately. EMON collection script adds -v parameter
:: which adds version and topology information to EMON_DATA specified above. So leave the following commented out
::set EMON_V=emon-v.dat
::set EMON_M=emon-m.dat

:: Workload related H/W and S/W configuration file; imported as-is into EDP spreadshe
set CONFIG_FILE=config.xlsx

:: Output of dmidecode; imported as-is into EDP spreadsheet
set DMIDECODE_FILE=dmidecode.txt

:: output of sar or other tool with network traffic
set NETWORKSTAT_FILE=network.txt

:: output of iostat or other tool with disk traffic
set DISKSTAT_FILE=diskstat.txt

:: output file name, you may want to change it
set OUTPUT_BASE=summary

:: the metrics definition file; need to change this based on the architecture
set SC_METRICS="Architecture_Specific\Client\Gracemont\grt-client.xml"
set BC_METRICS="Architecture_Specific\Client\Goldencove\glc-client.xml"

:: Excel chart format file, Need to change it based on the architecture
set SC_CHART_FORMAT="Architecture_Specific\Client\Gracemont\chart_format_grt_client.txt"
set BC_CHART_FORMAT="Architecture_Specific\Client\Goldencove\chart_format_glc_client.txt"

:: TSC_FREQ, if it was not provided in emon-v
::set SC_TSC_FREQ=2000
::set BC_TSC_FREQ=2000

:: the average value will be calculated from the %BEGIN% sample to %END% sample.
:: setting %END% to a negative value means the last availabe sample.
set BEGIN=1
set END=-1
::set BEGIN=1
::set END=-1
:: The BEGIN/END could also be a wall clock time in the format of mm/dd/yyyy hh:mm:ss.mmm
:: EMON data must be collected with timestamps (-c parameter) in this case. 
::set BEGIN="08/24/2012 17:53:20.885"
::set END="08/24/2012 17:53:35.885"

:: by default only system view will be outputted
:: there are 3 optional views to be selected
:: you can select one or more of them
set VIEW=--socket-view --core-view
::set VIEW=--socket-view --core-view --thread-view
::set VIEW=

::set the throughput (Transaction per Second)
::set TPS=--tps 10
::set TSC_FREQ=

:: if timestamps are presented in EMON data, the charts can be plotted with time as the x-axis.
:: by default the sample number is used as the x-axis. Don't enable this if timestamp data is not present in EMON data
::SET TIMESTAMP_IN_CHART="--timestamp-in-chart"
::set TIMESTAMP_IN_CHART=

set "SPLIT_CORE=SplitDat"
set "SPLIT_SMALL=%SPLIT_CORE%_smallcore"
set "SPLIT_BIG=%SPLIT_CORE%_bigcore"

%ruby% SplitCoreType.rb -i %EMON_DATA% -o "%SPLIT_CORE%"

if defined SC_TSC_FREQ set "SC_TSC_OPT= -c %SC_TSC_FREQ%"
if defined BC_TSC_FREQ set "BC_TSC_OPT= -c %BC_TSC_FREQ%"
if defined EMON_V set "EMON_V_OPT= -j %EMON_V%"
if defined EMON_M set "EMON_M_OPT= -k %EMON_M%"
set "OPTIONAL_SC=%EMON_V_OPT%%EMON_M_OPT%%SC_TSC_OPT%"
set "OPTIONAL_BC=%EMON_V_OPT%%EMON_M_OPT%%BC_TSC_OPT%"

%ruby% edp.rb -i %SPLIT_SMALL%  -g %CONFIG_FILE% -d %DMIDECODE_FILE% -D %DISKSTAT_FILE% -n %NETWORKSTAT_FILE% -f %SC_CHART_FORMAT% -o %OUTPUT_BASE%_smallcore.xlsx -m %SC_METRICS% -b %BEGIN% -e %END% %VIEW% %TPS% %TIMESTAMP_IN_CHART% %OPTIONAL_SC%
%ruby% edp.rb -i %SPLIT_BIG%  -g %CONFIG_FILE% -d %DMIDECODE_FILE% -D %DISKSTAT_FILE% -n %NETWORKSTAT_FILE% -f %BC_CHART_FORMAT% -o %OUTPUT_BASE%_bigcore.xlsx -m %BC_METRICS% -b %BEGIN% -e %END% %VIEW% %TPS% %TIMESTAMP_IN_CHART% %OPTIONAL_BC%
