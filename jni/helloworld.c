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

//using namespace std;
//
//

#define ARRAY_SIZE (1024)

#define  KERNEL_SRC "__kernel void helloworld(__global char* in, __global char* out) {"\
"	int num = get_global_id(0);"\
"	out[num] = in[num] + 1;"\
"}"

/* #define  KERNEL_SRC2 "__kernel void helloworld(int elems, __global char* in, __global char* out) {"\ */
/* "	int num = get_global_id(0);"\ */
/* "	out[0] = in[0] + 1;"\ */
/* "}" */

#define  KERNEL_SRC2 "__kernel void helloworld(int elems, __global char* in, __global char* out) {"\
"	int num = get_global_id(0);"\
"	int i;"\
"	for (i = 0; i < elems; i++) {"\
"	    out[i] = in[i] + 1;"\
"	}"\
"}"

#define CHECK_STATUS(status, msg) \
if (status != CL_SUCCESS) { \
    printf("ERROR %s line %d: " msg "\n", __FILE__, __LINE__); \
    exit(EXIT_FAILURE); \
} else { \
    printf("SUCCESS: " msg ", status=%d\n", status); \
} \

    /* printf("SUCCESS: " msg ", status=%d\n", status); \ */


#define PP(p)
//#define PP(p) printf(#p " pointer:%x\n",p)

#define NUM_WORKERS 1

int main(int argc, char* argv[])
{

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
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
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
	printf("input string: %s\n",input);
	char *output = (char*) malloc(strlength + 1);

	IAH();
	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (strlength + 1) * sizeof(char),(void *) input, &status);
    CHECK_STATUS(status, "clCreateBuffer");
	IAH();
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY , (strlength + 1) * sizeof(char), NULL, &status);
    CHECK_STATUS(status, "clCreateBuffer");

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
	
	/*Step 10: Running the kernel.*/
	/* size_t global_work_size[1] = {strlength}; */
	size_t global_work_size[1] = {NUM_WORKERS};
	IAH();
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
    CHECK_STATUS(status, "clEnqueueNDRangeKernel");

	/*Step 11: Read the cout put back to host memory.*/
	IAH();
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, strlength * sizeof(char), output, 0, NULL, NULL);
    CHECK_STATUS(status, "clEnqueueReadBuffer");
	
	output[strlength] = '\0';//Add the terminal character to the end of output.
	printf("output string: %s\n",output);

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

