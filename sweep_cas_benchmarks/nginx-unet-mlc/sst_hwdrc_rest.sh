#!/bin/bash
specdir="/home/speccpu/icc21"

function setup_env() {
  #cpupower frequency-set -u 2800000 -d 2800000
  #cpupower frequency-set -u 1800000 -d 1800000
  #cpupower frequency-set -u 2300000 -d 2300000
  wrmsr -a 0x774 0xff00
  wrmsr -a 0x620 0x0818
  ulimit -n 65536
  sst_reset
  hwdrc_disable 
  pqos -R
  
  rm -rf /root/.ssh/known_hosts
  rm -rf $specdir/result/
  
  echo off > /sys/devices/system/cpu/smt/control
  sleep 5 
  
  #destroy_vms
}

function setup_forbasemem() {
  #cpupower frequency-set -u 2800000 -d 2800000
  #cpupower frequency-set -u 1800000 -d 1800000
  cpupower frequency-set -u 2300000 -d 2300000
  #wrmsr -a 0x774 0xff00
  wrmsr -a 0x620 0x1414
  sst_reset
  hwdrc_disable 
  pqos -R
  
  rm -rf /root/.ssh/known_hosts
  
  echo off > /sys/devices/system/cpu/smt/control
  sleep 5 
  
  #destroy_vms
}

function sst_config() {
  # reset to default staete before applying the changes
  sst_reset

  HPCORE_RANGE=$1
  LPCORE_RANGE=$2
  hp_min_freq=$3
  lp_mx_freq=$4
  
  echo "Configuring SST ... "

  # Associate cores to CLOS groups
  intel-speed-select -c $HPCORE_RANGE core-power assoc -c 0
  intel-speed-select -c $LPCORE_RANGE core-power assoc -c 3
  
  #Enable SST
  intel-speed-select -c $(lscpu |grep node0 | cut -f2 -d:) core-power enable --priority 1

  #Set the CLOS parameters. Frequency for HP is 3000 Mhz and for LP is 1000 
  intel-speed-select -c 0 core-power config --clos 0 --min $hp_min_freq # max is 3100
  intel-speed-select -c 0 core-power config --clos 3 --min 0 --max $lp_mx_freq
}

function sst_config_tf() {
  # reset to default staete before applying the changes
  sst_reset

  HPCORE_RANGE=$1
  LPCORE_RANGE=$2
  hp_min_freq=$3
  lp_mx_freq=$4
  
  echo "Configuring SST ... "

  # Associate cores to CLOS groups
  intel-speed-select -c $HPCORE_RANGE core-power assoc -c 0
  intel-speed-select -c $LPCORE_RANGE core-power assoc -c 3
  
  #Enable core-power
  intel-speed-select -c $(lscpu |grep node0 | cut -f2 -d:) core-power enable --priority 1
  #eNABLE tf
  intel-speed-select -c $(lscpu |grep node0 | cut -f2 -d:) turbo-freq enable --priority 1

  #Set the CLOS parameters. Frequency for HP is 3000 Mhz and for LP is 1000 
  intel-speed-select -c 0 core-power config --clos 0 --min $hp_min_freq # max is 3100
  intel-speed-select -c 0 core-power config --clos 3 --min 0 --max $lp_mx_freq
}
# disable sst
function sst_reset() {
  echo "Reseting SST config ....."
  # Reset 	
  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) core-power config -c 0 &> /dev/null
  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) core-power config -c 1 &> /dev/null
  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) core-power config -c 2 &> /dev/null
  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) core-power config -c 3 &> /dev/null
  #Added following line
  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) core-power assoc -c 0 &> /dev/null

  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) core-power disable &> /dev/null
  intel-speed-select -c $(lscpu | grep node0 | cut -f2 -d:) turbo-freq disable &> /dev/null
}

function hwdrcequalPR_enable() {
  pqos -R
HWDRC_CAS=$1
HPCORE_RANGE=$2
LPCORE_RANGE=$3

  # enable HWDRC
  cd $PWD/hwdrc_postsi/scripts
  ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS_equalPR.sh $HWDRC_CAS $HPCORE_RANGE $LPCORE_RANGE
  cd -

  }
function hwdrc_enable() {
  pqos -R
HWDRC_CAS=$1
HPCORE_RANGE=$2
LPCORE_RANGE=$3

  # enable HWDRC
  cd $PWD/hwdrc_postsi/scripts
  ./hwdrc_icx_2S_xcc_init_to_default_pqos_CAS.sh $HWDRC_CAS $HPCORE_RANGE $LPCORE_RANGE
  cd -

  }

  function hwdrc_disable() {

  echo "Disable HWDRC .... "
  cd $PWD/hwdrc_postsi/scripts
  ./hwdrc_icx_2S_xcc_disable.sh

  pqos -R
  cd -
  }
