#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

// Size of the matrices - K, M, N (squared)
#define SIZE 3072

// Threadblock sizes (e.g. for kernel)
#define TS 16

#define MAX_SOURCE_SIZE (0x100000)
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

#define COUNT_RUNS 12

const cl_platform_info attributeTypes[5] = {
    CL_PLATFORM_NAME,
    CL_PLATFORM_VENDOR,
    CL_PLATFORM_VERSION,
    CL_PLATFORM_PROFILE,
    CL_PLATFORM_EXTENSIONS
};

const char* const attributeNames[] = {
    "CL_PLATFORM_NAME",
    "CL_PLATFORM_VENDOR",
    "CL_PLATFORM_VERSION",
    "CL_PLATFORM_PROFILE",
    "CL_PLATFORM_EXTENSIONS"
};

void PrintDeviceInfo(cl_device_id device)
{
    char queryBuffer[1024];
    int queryInt;
    cl_int clError;
    clError = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("    CL_DEVICE_NAME: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("    CL_DEVICE_VENDOR: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("    CL_DRIVER_VERSION: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(queryBuffer), &queryBuffer, NULL);
    printf("    CL_DEVICE_VERSION: %s\n", queryBuffer);
    queryBuffer[0] = '\0';
    clError = clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int), &queryInt, NULL);
    printf("    CL_DEVICE_MAX_COMPUTE_UNITS: %d\n", queryInt);
}

double performCalculation(int num_platform){
    // Set the sizes
    int K = SIZE;
    int M = SIZE;
    int N = SIZE;

    // Create the matrices and initialize them with random values
    float* A = (float*)malloc(M*K*sizeof(float*));
    float* B = (float*)malloc(K*N*sizeof(float*));
    float* C = (float*)malloc(M*N*sizeof(float*));
    for (int i=0; i<M*K; i++) { A[i] = 3.6*i + i*i + 3.1; }
    for (int i=0; i<K*N; i++) { B[i] = 1.2*i + 0.01*i*i + 13.9; }
    for (int i=0; i<M*N; i++) { C[i] = 0.0; }
    

    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;
       

    fp = fopen("matrix_multiply_kernel.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Get platform and device information
    cl_platform_id * platforms = NULL;
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms = 0;
    

    cl_int err = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    //printf("\n Count of platforms: %d \n", ret_num_platforms);

    platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id) * ret_num_platforms);
    err = clGetPlatformIDs(ret_num_platforms, platforms, NULL);

    err = clGetDeviceIDs(platforms[num_platform], CL_DEVICE_TYPE_ALL, 1, &device_id, &ret_num_devices);

    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &err);

    // Create a command queue with profiling
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);

    // Create memory buffers on the device for each matrix 
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,  M*K*sizeof(float), NULL, NULL);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY,  K*N*sizeof(float), NULL, NULL);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, M*N*sizeof(float), NULL, NULL);

    // Copy matrices to the GPU
    err = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, M*K*sizeof(float), A, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, K*N*sizeof(float), B, 0, NULL, NULL);
    err = clEnqueueWriteBuffer(command_queue, c_mem_obj, CL_TRUE, 0, M*N*sizeof(float), C, 0, NULL, NULL);

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, 
            (const char **)&source_str, (const size_t *)&source_size, &err);

    // Build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "matrix_multiply", &err);

    // Set the arguments of the kernel
    err = clSetKernelArg(kernel, 0, sizeof(int), (void*)&M);
    err = clSetKernelArg(kernel, 1, sizeof(int), (void*)&N);
    err = clSetKernelArg(kernel, 2, sizeof(int), (void*)&K);
    err = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&a_mem_obj);
    err = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&b_mem_obj);
    err = clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&c_mem_obj);
    
    
    //init event
    cl_event event;
    // Execute the OpenCL kernel 
    const size_t local[2] = { TS, TS};
    const size_t global[2] = { M, N };
    err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    
    
    //wait the kernel finish
    clWaitForEvents(1, &event);

    // Read the memory buffer C on the device to the local variable C
    err = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, M*N*sizeof(float), C, 0, NULL, NULL);

    // Clean up
    err = clFlush(command_queue);
    err = clFinish(command_queue);
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(a_mem_obj);
    err = clReleaseMemObject(b_mem_obj);
    err = clReleaseMemObject(c_mem_obj);
    err = clReleaseCommandQueue(command_queue);
    err = clReleaseContext(context);
    free(A);
    free(B);
    free(C);

    //Get profiling data and calculate the kernel execution time (returned by the OpenCL API in nanoseconds)
    cl_ulong time_start;
    cl_ulong time_end;

    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);

    double nanoSeconds = time_end-time_start;
    double milliseconds = nanoSeconds / 1000000.0;
    printf("OpenCl Execution time is: %0.3f milliseconds \n",milliseconds);
    
    return milliseconds;
}


int main(void) {
	int i, j, k, num_attributes;
    char* info;

    cl_platform_id * platforms = NULL;
    cl_uint num_platforms;
    cl_device_id *device_list = NULL;
    cl_uint num_devices;
    cl_int clStatus;
    size_t infoSize;

	// Get platform and device information
    clStatus = clGetPlatformIDs(0, NULL, &num_platforms);
    platforms = (cl_platform_id *) malloc(sizeof(cl_platform_id) * num_platforms);
    clStatus = clGetPlatformIDs(num_platforms, platforms, NULL);

    // for each platform print all attributes
    num_attributes = NELEMS(attributeTypes);
    printf("\nAttribute Count = %d ", num_attributes);
    for (i = 0; i < num_platforms; i++) {
        printf("Platform - %d\n", i+1);
        printf("\n Platform id: %d \n", platforms[i]);
        for (j = 0; j < num_attributes; j++) {
            // get platform attribute value size
            clGetPlatformInfo(platforms[i], attributeTypes[j], 0, NULL, &infoSize);
            info = (char*) malloc(infoSize);
            // get platform attribute value
            clGetPlatformInfo(platforms[i], attributeTypes[j], infoSize, info, NULL);
            printf("  %d.%d %-11s: %s\n", i+1, j+1, attributeNames[j], info);
        }
        //Get the devices list and choose the device you want to run on
        clStatus = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL, &num_devices);
        device_list = (cl_device_id *) malloc(sizeof(cl_device_id)*num_devices);
        clStatus = clGetDeviceIDs( platforms[i], CL_DEVICE_TYPE_GPU, num_devices, device_list, NULL);
        for (k = 0; k < num_devices; k++) {
            printf("  Device - %d:\n", (k+1));
            PrintDeviceInfo(device_list[k]);
            
        }
    }

    double time_execution =0.0;
    double average_time = 0.0;

    for (int i=0;i<num_platforms;i++){
        printf("\n Current platform id: %d \n", platforms[i]);
        //cold start
        time_execution=performCalculation(i);

        for (int j =0;j<COUNT_RUNS;j++){
            time_execution=performCalculation(i);
            average_time+=time_execution;
        }
        average_time=average_time / COUNT_RUNS;
        printf("Measure execution time is: %0.5f milliseconds \n", average_time);
        average_time=0.0;

    }
    free(platforms);
    free(device_list);
    
    return 0;
}

