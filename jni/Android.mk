# To make:
# ndk-build                      // defaults to helloworld 
# ndk-build PROGRAM=empty_kernel // builds empty_kernel    

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_LDLIBS := -ldl
LOCAL_CFLAGS := -g
LOCAL_CXXFLAGS += -fno-exceptions

COMMON_STUFF := aopencl.c libhelloworld.c clutils.c

ifndef $(PROGRAM)
	PROGRAM := helloworld
endif

LOCAL_MODULE    := $(PROGRAM)
LOCAL_SRC_FILES := $(COMMON_STUFF) $(PROGRAM).c

#include $(BUILD_SHARED_LIBRARY)  
include $(BUILD_EXECUTABLE)
