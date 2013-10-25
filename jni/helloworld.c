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

//using namespace std;
//
//

#define MAX_CHARS "1000"

#define ARRAY_SIZE (1024)
/* A sufficiently large ARRAY_SIZE will cause the device to crash and reboot (it will get past 
 * allocating and writig the input and allocating output memory, but crashes is mid-kernel 
 * execution).
 */
// #define ARRAY_SIZE (5000 * 1024)

#define  KERNEL_SRC "__kernel void helloworld(__global char* in, __global char* out) {"\
"	int num = get_global_id(0);"\
"	out[num] = in[num] + 1;"\
"}"

/* #define  KERNEL_SRC2 "__kernel void helloworld(int elems, __global char* in, __global char* out) {"\ */
/* "	int num = get_global_id(0);"\ */
/* "	out[0] = in[0] + 1;"\ */
/* "}" */

#define KERNEL_SRC2 "__kernel void helloworld(int elems, __global char* in, __global char* out) {"\
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

#define CHECK_STATUS(status, msg) \
if (status != CL_SUCCESS) { \
    printf("ERROR %s line %d: " msg ": %s\n", __FILE__, __LINE__, clErrorString(status)); \
    exit(EXIT_FAILURE); \
} else { \
    printf("SUCCESS: " msg ", status=%d\n", status); \
} \

    /* printf("SUCCESS: " msg ", status=%d\n", status); \ */


#define PP(p)
//#define PP(p) printf(#p " pointer:%x\n",p)

#define NUM_WORKERS 1


inline double ms(cl_ulong t) {
    return t * 1e-06;
}


void profile_times(cl_int* status, cl_event event, cl_ulong* start, cl_ulong* end, cl_ulong* duration);
int main(int argc, char* argv[])
{


    /* glFinish(); */
    /* status = clEnqueueAcquireGLObjects(commandQueue, 1, &cl_tex_mem, */
    /*         0,NULL,NULL ); */
    /* status = clEnqueueNDRangeKernel(commandQueue, tex_kernel, 2, NULL, */
    /*         tex_globalWorkSize, */
    /*         tex_localWorkSize, */
    /*         0, NULL, NULL); */
    /* clFinish(commandQueue); */
    /* status = clEnqueueReleaseGLObjects(commandQueue, 1, &cl_tex_mem, 0, NULL, NULL ); */

	/*Step1: Getting platforms and choose an available one.*/
	initFns();
	/* printf("HELLO\n"); */
	cl_uint numPlatforms;//the NO. of platforms
	cl_platform_id platform = NULL;//the chosen platform
	IAH();
	PP(clGetPlatformIDs);
	cl_int	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (status != CL_SUCCESS)
	{
		printf("Error: Getting platforms!\n");
		return 1;
	}

	/*For clarity, choose the first available platform. */
	if(numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id* )malloc(numPlatforms* sizeof(cl_platform_id));
		IAH();
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	/*Step 2:Query the platform and choose the first GPU device if has one.Otherwise use the CPU as device.*/
	cl_uint				numDevices = 0;
	cl_device_id        *devices;
	IAH();
	PP(clGetDeviceIDs);
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);	
	if (numDevices == 0) //no GPU available.
	{
		printf("No GPU device available.\n");
		printf("Choose CPU as default device.\n");
		IAH();
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);	
        CHECK_STATUS(status, "clGetDeviceIDs");
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		IAH();
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
        CHECK_STATUS(status, "clGetDeviceIDs");
	}
	else
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		IAH();
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
        CHECK_STATUS(status, "clGetDeviceIDs");
	}
	

	/*Step 3: Create context.*/
	IAH();
	cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
    CHECK_STATUS(status, "clCreateContext");
	
	/*Step 4: Creating command queue associate with the context.*/
	IAH();
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &status);
    CHECK_STATUS(status, "clCreateCommandQueue");

	/*Step 5: Create program object */
	//const char *filename = "HelloWorld_Kernel.cl";
	//string sourceStr;
	//status = convertToString(filename, sourceStr);
	/* const char *source = KERNEL_SRC;//sourceStr.c_str(); */
	const char *source = KERNEL_SRC2;//sourceStr.c_str();
	size_t sourceSize[] = {strlen(source)};
	IAH();
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, &status);
    CHECK_STATUS(status, "clCreateProgramWithSource");
	
	/*Step 6: Build program. */
	IAH();
	status=clBuildProgram(program, 1,devices,NULL,NULL,NULL);
    CHECK_STATUS(status, "clBuildProgram");
	/* printf("HELLO\n"); */

	/*Step 7: Initial input,output for the host and create memory objects for the kernel*/
	/* const char* input = "GdkknVnqkc"; */
	char input[ARRAY_SIZE - 1];
	/* size_t strlength = strlen(input); */
	size_t strlength = ARRAY_SIZE - 1;
    int i;
    for (i = 0; i < strlength - 1; i++) {
        input[i] = '1';
    }
    input[strlength - 1] = '\0';
	printf("input string: %." MAX_CHARS "s\n",input);
	char *output = (char*) malloc(strlength + 1);

	IAH();
    cl_event cpu_to_gpu_event;
    /* NOTE:
     * CL_MEM_COPY_HOST_PTR => copy immediately
     * CL_MEM_USE_HOST_PTR => "OpenCL implementations are allowed to cache the buffer contents 
     * pointed to by host_ptr in device memory. This cached copy can be used when kernels are 
     * executed on a device."
     *
     * May be worth investigating performance differences of these approaches.
     */
	/* cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (strlength + 1) * sizeof(char),(void *) input, &status); */
	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (strlength + 1) * sizeof(char), NULL, &status);
    CHECK_STATUS(status, "clCreateBuffer");
	IAH();
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY , (strlength + 1) * sizeof(char), NULL, &status);
    CHECK_STATUS(status, "clCreateBuffer");
    status = clEnqueueWriteBuffer(commandQueue, inputBuffer, 
            CL_TRUE, // blocking write
            0, (strlength + 1) * sizeof(char), input, 0, NULL, &cpu_to_gpu_event);
    CHECK_STATUS(status, "clEnqueueWriteBuffer");
    cl_ulong cpu_to_gpu_event_start, cpu_to_gpu_event_end, cpu_to_gpu_event_duration;
    profile_times(&status, cpu_to_gpu_event, &cpu_to_gpu_event_start, &cpu_to_gpu_event_end, &cpu_to_gpu_event_duration);

	/*Step 8: Create kernel object */
	IAH();
	cl_kernel kernel = clCreateKernel(program,"helloworld", &status);
    CHECK_STATUS(status, "clCreateKernel");

	/*Step 9: Sets Kernel arguments.*/
	/* IAH(); */
	/* status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer); */
	/* IAH(); */
	/* status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer); */



    /* ciErr1 |= clSetKernelArg(ckKernel, 3, sizeof(cl_int), (void*)&iNumElements); */
	/* printf("HELLO\n",input); */
	/*Step 9: Sets Kernel arguments.*/
	IAH();
    int strsize = strlength;
	status = clSetKernelArg(kernel, 0, sizeof(cl_int), (void *)&strsize);
    CHECK_STATUS(status, "clSetKernelArg");
	IAH();
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&inputBuffer);
    CHECK_STATUS(status, "clSetKernelArg");
	IAH();
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&outputBuffer);
    CHECK_STATUS(status, "clSetKernelArg");

    cl_event kernel_event;
	
	/*Step 10: Running the kernel.*/
	/* size_t global_work_size[1] = {strlength}; */
	size_t global_work_size[1] = {NUM_WORKERS};
	IAH();
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, &kernel_event);
    CHECK_STATUS(status, "clEnqueueNDRangeKernel");
    
    // Wait for the commands to complete before reading back results
    clFinish(commandQueue);

    /* Collect profiling data:
     * start end end times of kernel execution.
     */
    cl_ulong kernel_event_start, kernel_event_end, kernel_event_duration;
    profile_times(&status, kernel_event, &kernel_event_start, &kernel_event_end, &kernel_event_duration);

	/* Step 11: 
     * Read back from the GPU to main memory.
     */
	IAH();
    cl_event gpu_to_cpu_event;
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, strlength * sizeof(char), output, 0, NULL, &gpu_to_cpu_event);
    CHECK_STATUS(status, "clEnqueueReadBuffer");
    cl_ulong gpu_to_cpu_event_start, gpu_to_cpu_event_end, gpu_to_cpu_event_duration;
    profile_times(&status, gpu_to_cpu_event, &gpu_to_cpu_event_start, &gpu_to_cpu_event_end, &gpu_to_cpu_event_duration);
	
	output[strlength] = '\0';//Add the terminal character to the end of output.
	printf("output string: %." MAX_CHARS "s\n",output);

    for (i = 0; i < strlength - 1; i++) {
        if (output[i] != '2') {
            printf("ERROR: expected '2' but saw %c at output[%i]\n", output[i], i);
            exit(EXIT_FAILURE);
        }
        input[i] = '1';
    }

    /* Print the maximum allocatable memory size for the device.
     */

    cl_bool device_available = CL_FALSE;
	status = clGetDeviceInfo(devices[0], CL_DEVICE_AVAILABLE, sizeof(cl_bool), &device_available, NULL);
    CHECK_STATUS(status, "clGetDeviceInfo");
    if (device_available != CL_TRUE) 
    {
		printf("Error: Device %i is not available\n", 0);
		return EXIT_FAILURE;
    }

    cl_ulong device_max_mem_alloc_size = 0;
	status = clGetDeviceInfo(devices[0], CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), &device_max_mem_alloc_size, NULL);
    CHECK_STATUS(status, "clGetDeviceInfo");
    cl_ulong device_global_mem_size = 0;
    
	status = clGetDeviceInfo(devices[0], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &device_max_mem_alloc_size, NULL);
    CHECK_STATUS(status, "clGetDeviceInfo");
	printf("The max allocateable memory size is %i\n", device_max_mem_alloc_size);
	printf("The size of global device memory is %i\n", device_global_mem_size);
    print_profile_times("kernel execution", kernel_event_start, kernel_event_end, kernel_event_duration);
    print_profile_times("CPU-to-GPU input array copy", cpu_to_gpu_event_start, cpu_to_gpu_event_end, cpu_to_gpu_event_duration);
    print_profile_times("GPU-to-CPU output array copy", gpu_to_cpu_event_start, gpu_to_cpu_event_end, gpu_to_cpu_event_duration);


	/*Step 12: Clean the resources.*/
	IAH();
	status = clReleaseKernel(kernel);//*Release kernel.
    CHECK_STATUS(status, "clReleaseKernel");
	IAH();
	status = clReleaseProgram(program);	//Release the program object.
    CHECK_STATUS(status, "clReleaseProgram");
	IAH();
	status = clReleaseMemObject(inputBuffer);//Release mem object.
    CHECK_STATUS(status, "clReleaseMemObject");
	IAH();
	status = clReleaseMemObject(outputBuffer);
    CHECK_STATUS(status, "clReleaseMemObject");
	IAH();
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
    CHECK_STATUS(status, "clReleaseCommandQueue");
	IAH();
	status = clReleaseContext(context);//Release context.
    CHECK_STATUS(status, "clReleaseContext");

	IAH();
	if (output != NULL)
	{
		IAH();
		free(output);
		output = NULL;
	}

	if (devices != NULL)
	{
		IAH();
		free(devices);
		devices = NULL;
	}

	IAH();
	return 0;
}

void profile_times(cl_int* status, cl_event event, cl_ulong* start, cl_ulong* end, cl_ulong* duration) {
    /* Collect profiling data:
     * start end end times of kernel execution.
     */
    *start=(cl_ulong)0;
    *end=(cl_ulong)0;
    /* A 64-bit value that describes the current device time counter in nanoseconds when the command 
     * identified by event starts execution on the device. 
     */
    *status = clGetEventProfilingInfo(event,
                                  CL_PROFILING_COMMAND_START,
                                  sizeof(cl_ulong),
                                  start,
                                  NULL);
    CHECK_STATUS(*status, "clGetEventProfilingInfo");
    /* A 64-bit value that describes the current device time counter in nanoseconds when the command 
     * identified by event has finished execution on the device. 
     */
    *status = clGetEventProfilingInfo(event,
                                  CL_PROFILING_COMMAND_END,
                                  sizeof(cl_ulong),
                                  end,
                                  NULL);
    CHECK_STATUS(*status, "clGetEventProfilingInfo");
    /* cl_ulong g_NDRangePureExecTimeMs = (cl_double)(end - start)*(cl_double)(1e-06);  */
    /* assert(end >= start); */
    assert(*end >= *start);
    *duration = end - start; 
}

void print_profile_times(const char * activity, cl_ulong start, cl_ulong end, cl_ulong duration) {
	printf("The time to complete %s was %f ms\n", activity, ms(duration));
	printf("    CL_PROFILING_COMMAND_START = %" PRIu64 " ns\n", start);
	printf("    CL_PROFILING_COMMAND_END = %" PRIu64 " ns\n", end);
}
