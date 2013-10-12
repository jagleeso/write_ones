# ./rl result/file.txt
set -x
set -e
opencl=helloworld.cl
executable=helloworld
ndk-build
adb push libs/armeabi-v7a/$executable /data/local/tmp
adb push jni/$opencl /data/local/tmp
adb shell ./data/local/tmp/$executable
adb push stress_test.sh /data/local/tmp
# adb shell ./data/local/tmp/opencl_aes
adb shell ./data/local/tmp/stress_test.sh | tee "$1"
