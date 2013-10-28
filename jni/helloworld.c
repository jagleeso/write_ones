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

#include "libhelloworld.h"

#undef NDEBUG
#include <assert.h>

#include <inttypes.h>

/* #define ARRAY_SIZE (1024) */

/* A sufficiently large ARRAY_SIZE will cause the device to crash and reboot (it will get past 
 * allocating and writig the input and allocating output memory, but crashes is mid-kernel 
 * execution).
 *
 * However, this no longer crashes when OpenGL shared libraries are not present, indicating there is some 
 * background OpenGL related GPU activity going on.
 */
// #define ARRAY_SIZE (5000 * 1024)

#define ARRAY_SIZE (7500 * 1024)

/* The regular program.
 */
#define KERNEL_SRC "__kernel void helloworld(int elems, __global char* in, __global char* out) {"\
"	int num = get_global_id(0);"\
"	int i;"\
"	for (i = 0; i < elems; i++) {"\
"	    out[i] = in[i] + 1;"\
"	}"\
"}"

/* NOTE:
 * Executing an infinite loop in the kernel causes the device to crash and reboot; not clear why 
 * this is the case...
"   for (; ; ) {"\
"   }"\
 */ 

/* NOTE:
 * Maybe the driver is checking for memory accesses to detect timeouts.  Try writing the output buffer 
 * repeatedly, or accessing the input buffer repeatedly; that is:
 * out[0] = '0'
 * out[0] = '1'
 * out[0] = '0'
 * ...
 * READ in[0]
 * READ in[1]
 * READ in[0]
 * ...
 *
"	int j = 0;"\
"	char read_char;"\
"   for (; ; ) {"\
"	    j += 1;"\
"	    j = j % 2;"\
"	    out[0] = (char) j;"\
"	    read_char = in[j];"\
"   }"\
 * STILL FAILS
 */ 

/* Perhaps this example is too simple (e.g. if it checks buffer read/write addresses, maybe we 
 * happen to fall on the same address)... lets loop over the whole output array repeatedly and flip 
 * between '1' and '2', and read the input array at each position:
"	char read_char;"\
"	for (i = 0; ; i = (i + 1) % elems) {"\
"	    int out_int = out[i];"\
"	    out_int = '1' + (( (out_int - '1') + 1 ) % 2);"\
"	    out[i] = (char) out_int;"\
"	    read_char = in[i];"\
"	}"\
 * STILL FAILS
 */

int main(int argc, char* argv[])
{
    benchmark_helloworld(argc, argv, KERNEL_SRC, ARRAY_SIZE, 1);
}

