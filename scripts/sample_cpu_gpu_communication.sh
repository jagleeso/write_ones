#!/bin/bash
# Sample the time it takes to copy memory to the GPU device (CPU-to-GPU), and copy GPU device memory 
# back into main memory (GPU-to-CPU).


SAMPLES=10

cd "$(dirname "$0")"
source ./common.sh
cd $ROOT
# set -x
set -e
executable=helloworld

max_array_size=""
guess_max_array_size() {
    build empty_kernel
    copy_program_over empty_kernel
    local increment=100000
    # local size=$((1024*7500))
    local size=$((1536*1024*1024))
    while true; do
        echo "SIZE == $size"
        if adb shell ./data/local/tmp/empty_kernel $size 2>&1 | grep --quiet "Segmentation fault"; then
            max_array_size=$((size - increment))
            echo "> Max input/output array size (bytes): $max_array_size"
            echo "> Error in max input/output (bytes): $increment"
            break
        fi
        size=$((size + increment))
    done
}

# guess_max_array_size
max_array_size=$((512*1024*1024))
echo "> Max input/output array size (bytes): $max_array_size"

INCREMENT=$((max_array_size / SAMPLES))
echo "> Sample input/ouput size increment (bytes): $INCREMENT"
echo "> Samples: $SAMPLES"

build $executable
copy_program_over $executable

array_size=$INCREMENT
for i in $(seq 1 $SAMPLES); do 
    adb shell ./data/local/tmp/$executable $array_size
    array_size=$((array_size + INCREMENT))
    echo
done
