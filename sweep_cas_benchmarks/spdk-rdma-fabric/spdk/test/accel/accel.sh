#!/usr/bin/env bash

testdir=$(readlink -f $(dirname $0))
rootdir=$(readlink -f $testdir/../..)
source $rootdir/test/common/autotest_common.sh

#Run through all SW ops with defaults for a quick sanity check
#To save time, only use verification case
run_test "accel_crc32c" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w crc32c -y
run_test "accel_crc32c_C2" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w crc32c -y -C 2
run_test "accel_copy" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w copy -y
run_test "accel_fill" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w fill -y
run_test "accel_copy_crc32c" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w copy_crc32c -y
run_test "accel_copy_crc32c_C2" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w copy_crc32c -y -C 2
run_test "accel_dualcast" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w dualcast -y
run_test "accel_compare" $SPDK_EXAMPLE_DIR/accel_perf -t 1 -w compare -y
