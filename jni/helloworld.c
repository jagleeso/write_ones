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

#define  KERNEL_SRC "__kernel void helloworld(__global char* in, __global char* out) {"\
"	int num = get_global_id(0);"\
"	out[num] = in[num] + 1;"\
"}"

#define PP(p)
//#define PP(p) printf(#p " pointer:%x\n",p)

int main(int argc, char* argv[])
{

	/*Step1: Getting platforms and choose an available one.*/
	initFns();
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
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		IAH();
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
	}
	else
	{
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

		IAH();
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}
	

	/*Step 3: Create context.*/
	IAH();
	cl_context context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
	
	/*Step 4: Creating command queue associate with the context.*/
	IAH();
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	/*Step 5: Create program object */
	//const char *filename = "HelloWorld_Kernel.cl";
	//string sourceStr;
	//status = convertToString(filename, sourceStr);
	const char *source = KERNEL_SRC;//sourceStr.c_str();
	size_t sourceSize[] = {strlen(source)};
	IAH();
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
	
	/*Step 6: Build program. */
	IAH();
	status=clBuildProgram(program, 1,devices,NULL,NULL,NULL);

	/*Step 7: Initial input,output for the host and create memory objects for the kernel*/
	const char* input = "GdkknVnqkc";
	size_t strlength = strlen(input);
	printf("input string: %s\n",input);
	char *output = (char*) malloc(strlength + 1);

	IAH();
	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (strlength + 1) * sizeof(char),(void *) input, NULL);
	IAH();
	cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY , (strlength + 1) * sizeof(char), NULL, NULL);

	/*Step 8: Create kernel object */
	IAH();
	cl_kernel kernel = clCreateKernel(program,"helloworld", NULL);

	/*Step 9: Sets Kernel arguments.*/
	IAH();
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
	IAH();
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&outputBuffer);
	
	/*Step 10: Running the kernel.*/
	size_t global_work_size[1] = {strlength};
	IAH();
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);

	/*Step 11: Read the cout put back to host memory.*/
	IAH();
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, strlength * sizeof(char), output, 0, NULL, NULL);
	
	output[strlength] = '\0';//Add the terminal character to the end of output.
	printf("output string: %s\n",output);

	/*Step 12: Clean the resources.*/
	IAH();
	status = clReleaseKernel(kernel);//*Release kernel.
	IAH();
	status = clReleaseProgram(program);	//Release the program object.
	IAH();
	status = clReleaseMemObject(inputBuffer);//Release mem object.
	IAH();
	status = clReleaseMemObject(outputBuffer);
	IAH();
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
	IAH();
	status = clReleaseContext(context);//Release context.

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

