#!/bin/bash
# Sample the time it takes for the GPU to timeout (as indicated by the phone crashing) on an OpenCL 
# program that executes an infinite loop.

SAMPLES=25
# seconds to wait for device to reboot after crashing
# SLEEP_UNTIL_REBOOT=5

cd "$(dirname "$0")"
cd ../
# set -x
set -e
opencl=helloworld.cl
executable=helloworld
ndk-build

copy_program_over() {
    adb push libs/armeabi-v7a/$executable /data/local/tmp
    adb push jni/$opencl /data/local/tmp
}

wait_for_device_reboot() {
    while true; do
        # poll until the device is available
        adb devices | \
            grep -v "List of devices attached" | \
            grep "device" --quiet && \
            break
        sleep 1
    done
}

seq 1 $SAMPLES | while read i; do 
    copy_program_over
    # http://stackoverflow.com/questions/2408981/how-cant-i-redirect-the-output-of-time-command
    { time adb shell ./data/local/tmp/$executable; } 2>&1
    echo
    wait_for_device_reboot
done
