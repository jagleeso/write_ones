/**********************************************************************
Copyright (c) 2013  GSS Mahadevan
Copyright ©2012 Advanced Micro Devices, Inc. All rights reserved.

********************************************************************/

//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <iostream>
//#include <string>
//#include <fstream>
#include "aopencl.h"
#include "clutils.h"

#undef NDEBUG
#include <assert.h>

#include <inttypes.h>

/* The most minimal OpenCL kernel.  Used to check the time returned by clGetEventProfilingInfo, and to what 
 * degree they could cause overestimates.
 */
#define KERNEL_SRC "__kernel void helloworld(int elems, __global char* in, __global char* out) {"\
"	int num = get_global_id(0);"\
"}"

int main(int argc, char* argv[])
{
    benchmark_helloworld(argc, argv, KERNEL_SRC, 100, 0);
}
