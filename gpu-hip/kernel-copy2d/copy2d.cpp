#include <hip/hip_runtime.h>
#include <stdio.h>
#include <math.h>

// TODO: add a device kernel that copies all elements of a vector
//       using GPU threads in a 2D grid
__global__ void copy_(int n, int m, double *x, double *y)
{
    int tid_x = threadIdx.x + blockIdx.x * blockDim.x;
    int stride_x = gridDim.x * blockDim.x;
    int tid_y = threadIdx.y + blockIdx.y * blockDim.y;
    int stride_y = gridDim.y * blockDim.y;

    //go over all sites, again using stride so dont need to worry about having enough threads?
    for(; tid_x < n; tid_x += stride_x){
        for(; tid_y < m; tid_y += stride_y){
            y[tid_x * m + tid_y] = x[tid_x * m + tid_y];
    }
    }
}


int main(void)
{
    int i, j;
    const int n = 600;
    const int m = 400;
    const int size = n * m;
    double x[size], y[size], y_ref[size];
    double *x_, *y_;

    // initialise data
    for (i=0; i < size; i++) {
        x[i] = (double) i / 1000.0;
        y[i] = 0.0;
    }
    // copy reference values (C ordered)
    for (i=0; i < n; i++) {
        for (j=0; j < m; j++) {
            y_ref[i * m + j] = x[i * m + j];
        }
    }

    // TODO: allocate vectors x_ and y_ on the GPU
    hipMalloc(&x_, sizeof(double)*size);
    hipMalloc(&y_, sizeof(double)*size);
    // TODO: copy initial values from CPU to GPU (x -> x_ and y -> y_)
    hipMemcpy(x_, x, sizeof(double)*size, hipMemcpyHostToDevice);
    hipMemcpy(y_, y, sizeof(double)*size, hipMemcpyHostToDevice);

    // TODO: define grid dimensions (use 2D grid!)
    dim3 blocks(16,2);
    dim3 threads(64,4);

    // TODO: launch the device kernel
    hipLaunchKernelGGL(copy_, blocks, threads, 0, 0, n, m, x_, y_);

    // TODO: copy results back to CPU (y_ -> y)
    hipMemcpy(y, y_, sizeof(double)*size, hipMemcpyDeviceToHost);

    // confirm that results are correct
    double error = 0.0;
    for (i=0; i < size; i++) {
        error += abs(y_ref[i] - y[i]);
    }
    printf("total error: %f\n", error);
    printf("  reference: %f at (42,42)\n", y_ref[42 * m + 42]);
    printf("     result: %f at (42,42)\n", y[42 * m + 42]);

    hipFree(x_);
    hipFree(y_);

    return 0;
}
