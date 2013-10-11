LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_LDLIBS := -ldl
LOCAL_MODULE    := helloworld
LOCAL_CFLAGS := -g
LOCAL_CXXFLAGS += -fno-exceptions
LOCAL_SRC_FILES := aopencl.c helloworld.c
#include $(BUILD_SHARED_LIBRARY)  
include $(BUILD_EXECUTABLE)
