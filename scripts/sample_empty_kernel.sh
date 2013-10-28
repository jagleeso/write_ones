#!/bin/bash
# Sample the time it takes for the GPU to timeout (as indicated by the phone crashing) on an OpenCL 
# program that executes an infinite loop.

SAMPLES=25
# seconds to wait for device to reboot after crashing
# SLEEP_UNTIL_REBOOT=5

cd "$(dirname "$0")"
source ./common.sh
cd $ROOT
# set -x
set -e
executable=empty_kernel

build $executable
copy_program_over $executable

seq 1 $SAMPLES | while read i; do 
    adb shell ./data/local/tmp/$executable
    echo
done
