#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>
#include "readVector.h"

#define MAX_SOURCE_SIZE (0x100000)

#define CHECK_ERR(err, msg) \
    if (err != CL_SUCCESS) { \
        fprintf(stderr, "%s failed: %d\n", msg, err); \
        exit(EXIT_FAILURE); \
    }

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_file_0> <input_file_1> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char* inputFile1 = argv[1];
    const char* inputFile2 = argv[2];
    const char* outputFile = argv[3];

    // Load kernel source code
    FILE *kernel_file;
    char *kernel_source;

    kernel_file = fopen("kernel.cl", "r");
    if (!kernel_file) {
        fprintf(stderr, "Failed to open kernel file.\n");
        exit(EXIT_FAILURE);
    }

    kernel_source = (char *)malloc(MAX_SOURCE_SIZE);
    fread(kernel_source, 1, MAX_SOURCE_SIZE, kernel_file);
    fclose(kernel_file);

    // Host input and output vectors and sizes
    float *h_a, *h_b, *h_c;
    int a_size, b_size, c_size;

    // Device input and output buffers 
    cl_mem d_a;
    cl_mem d_b;
    cl_mem d_c;

    size_t global_item_size, local_item_size;
    cl_int err;

    cl_platform_id cpPlatform;        // OpenCL platform
    cl_device_id device_id;           // device ID
    cl_context context;               // context
    cl_command_queue queue;           // command queue
    cl_program program;               // program
    cl_kernel kernel;                 // kernel

    err = readVector(inputFile1, &h_a, &a_size);
    if (err != CL_SUCCESS)
        exit(EXIT_FAILURE);
    err = readVector(inputFile2, &h_b, &b_size);
    if (err != CL_SUCCESS)
        exit(EXIT_FAILURE);
    err = readVector(outputFile, &h_c, &c_size);
    if (err != CL_SUCCESS)
        exit(EXIT_FAILURE);

    printf("Vector Length: %d\n", a_size);

    // Bind to platform
    err = clGetPlatformIDs(1, &cpPlatform, NULL);
    CHECK_ERR(err, "clGetPlatformIDs");

    // Get ID for the device
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    CHECK_ERR(err, "clGetDeviceIDs");

    // Create a context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    CHECK_ERR(err, "clCreateContext");

    // Create a command queue
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    CHECK_ERR(err, "clCreateCommandQueue");

    // Create the program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, NULL, &err);
    CHECK_ERR(err, "clCreateProgramWithSource");

    // Build the program executable
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    CHECK_ERR(err, "clBuildProgram");

    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, "vectorAdd", &err);
    CHECK_ERR(err, "clCreateKernel");

    //@@ Allocate GPU memory here
    // Create memory buffers for input and output vectors
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, a_size * sizeof(float), NULL, &err);
    CHECK_ERR(err, "clCreateBuffer d_a");
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, b_size * sizeof(float), NULL, &err);
    CHECK_ERR(err, "clCreateBuffer d_b");
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, c_size * sizeof(float), NULL, &err);
    CHECK_ERR(err, "clCreateBuffer d_c");

    //@@ Copy memory to the GPU here
    // Copy input vectors to memory buffers
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, a_size * sizeof(float), h_a, 0, NULL, NULL);
    CHECK_ERR(err, "clEnqueueWriteBuffer d_a");
    err = clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, b_size * sizeof(float), h_b, 0, NULL, NULL);
    CHECK_ERR(err, "clEnqueueWriteBuffer d_b");

    // Set the arguments to our compute kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    CHECK_ERR(err, "clSetKernelArg 0");
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    CHECK_ERR(err, "clSetKernelArg 1");
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
    CHECK_ERR(err, "clSetKernelArg 2");
    err |= clSetKernelArg(kernel, 3, sizeof(int), &a_size);
    CHECK_ERR(err, "clSetKernelArg 3");

    //@@ Initialize the global size and local size here
    global_item_size = a_size;
    local_item_size = 1;

    //@@ Launch the GPU Kernel here
    // Execute the OpenCL kernel on the array
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
    CHECK_ERR(err, "clEnqueueNDRangeKernel");

    // Wait for the command queue to get serviced before reading back results
    clFlush(queue);
    clFinish(queue);

    //@@ Copy the GPU memory back to the CPU here
    // Read the memory buffer output_mem_obj to the local variable result
    float *result = (float *)malloc(c_size * sizeof(float));
    err = clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, c_size * sizeof(float), result, 0, NULL, NULL);

    // THIS NEEDS TO BE MADE INTO A HELPER LIBRARY FUCNTION =====================================================================|
    // Compare result with existing_result if it exists                                                                       // |
    if (c_size == a_size) {                                                                                                   // |
        float tolerance = 1e-4;                                                                                               // |
        int different = 0;                                                                                                    // |
        for (int i = 0; i < a_size; i++) {                                                                                    // |
            if (fabs(result[i] - h_c[i]) > tolerance) {                                                                       // |
                different = 1;                                                                                                // |
                printf("FAILED: Difference found at index %d: %.2f (actual) != %.2f (expected)\n", i, result[i], h_c[i]);     // |
            }                                                                                                                 // |
        }                                                                                                                     // |
        if (!different) {                                                                                                     // |
            printf("PASSED\n");                                                                                               // |
            return EXIT_SUCCESS;                                                                                              // |
        }                                                                                                                     // |
    }                                                                                                                         // |
    else {                                                                                                                    // |
        printf("FAILED: Actual and expected vector sizes do not match.\n");                                                   // |
    }                                                                                                                         // |
    // ==========================================================================================================================|

    //@@ Free the GPU memory here
    // Release OpenCL resources
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // Release host memory
    free(h_a);
    free(h_b);
    free(h_c);
    free(kernel_source);
    free(result);
    
    return 0;
}
