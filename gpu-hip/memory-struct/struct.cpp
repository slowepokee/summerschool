#include <cstdio>
#include <hip/hip_runtime.h>
#include <time.h>

/* Blocksize divisible by the warp size */
#define BLOCKSIZE 64

/* Example struct to practise copying structs with pointers to device memory */
typedef struct
{
  float *x;
  int *idx;
  int size;
} Example;

/* GPU kernel definition */
__global__ void hipKernel(Example* const d_ex)
{
  const int thread = blockIdx.x * blockDim.x + threadIdx.x;
  if (thread < d_ex->size)
  {
    printf("x[%d]: %.2f, idx[%d]:%d/%d \n", thread, d_ex->x[thread], thread, d_ex->idx[thread], d_ex->size - 1);
  }
}

/* Run on host */
void runHost()
{
  // Allocate host struct
  Example *ex;
  ex = (Example*)malloc(sizeof(Example));
  ex->size = 10;

  // Allocate host struct members
  ex->x = (float*)malloc(ex->size * sizeof(float));
  ex->idx = (int*)malloc(ex->size * sizeof(int));

  // Initialize host struct members
  for(int i = 0; i < ex->size; i++)
  {
    ex->x[i] = (float)i;
    ex->idx[i] = i;
  }

  // Print struct values from host
  printf("\nHost:\n");
  for(int i = 0; i < ex->size; i++)
  {
    printf("x[%d]: %.2f, idx[%d]:%d/%d \n", i, ex->x[i], i, ex->idx[i], ex->size - 1);
  }

  // Free host struct
  free(ex->x);
  free(ex->idx);
  free(ex);
}

/* Run on device using Unified Memory */
void runDeviceUnifiedMem()
{
  
  Example *ex;
  hipMallocManaged((void**)&ex,sizeof(Example));
  ex->size = 10;

  
  hipMallocManaged((void**)&ex->x, ex->size*sizeof(float));
  hipMallocManaged((void**)&ex->idx, ex->size*sizeof(int));

  // Initialize struct from host
  for(int i = 0; i < ex->size; i++)
  {
    ex->x[i] = (float)i;
    ex->idx[i] = i;
  }
  //no idea where this gridsize comes from
  //const int gridsize = (ex->size - 1 + BLOCKSIZE) / BLOCKSIZE;
  const int gridsize = BLOCKSIZE;
  printf("\nDevice (UnifiedMem):\n");
  hipLaunchKernelGGL(hipKernel, gridsize, BLOCKSIZE, 0, 0, ex);
  hipStreamSynchronize(0);
  

  hipFree(ex->x);
  hipFree(ex->idx);
  hipFree(ex);
}

/* Create the device struct (needed for explicit memory management) */
Example* createDeviceExample(Example *ex)
{
  Example *d_ex;
  hipMalloc((void **)&d_ex, sizeof(Example));

  //pointers on **device**
  float *d_x;
  int *d_idx;

  //allocate space for pointers on device
  hipMalloc((void **)&d_x, ex->size*sizeof(float));
  hipMalloc((void **)&d_idx, ex->size*sizeof(int));
  
  //copy the values over
  hipMemcpy(d_x, ex->x, ex->size * sizeof(float), hipMemcpyHostToDevice);
  hipMemcpy(d_idx, ex->idx, ex->size * sizeof(int), hipMemcpyHostToDevice);

  //pointers are still in cpu memory -> copy to device
  //copy the pointers over
  hipMemcpy(&(d_ex->x), &d_x, sizeof(float*), hipMemcpyHostToDevice);
  hipMemcpy(&(d_ex->idx), &d_idx, sizeof(int*), hipMemcpyHostToDevice);
  hipMemcpy(&(d_ex->size), &(ex->size), sizeof(int), hipMemcpyHostToDevice);

  return d_ex;
}

/* Free the device struct (needed for explicit memory management) */
void freeDeviceExample(Example *d_ex)
{
  int *d_idx;
  float *d_x;

  //pointers are on gpu memory -> cant free without copying over to host
  hipMemcpy(&d_x, &(d_ex->x),sizeof(float*), hipMemcpyDeviceToHost);
  hipMemcpy(&d_idx, &(d_ex->idx), sizeof(int*), hipMemcpyDeviceToHost);


  hipFree(d_x);
  hipFree(d_idx);
  hipFree(d_ex);
}

/* Run on device using Explicit memory management */
void runDeviceExplicitMem()
{
  Example *ex;
  ex = (Example*)malloc(sizeof(Example));
  ex->size = 10;

  ex->x = (float*)malloc(ex->size * sizeof(float));
  ex->idx = (int*)malloc(ex->size * sizeof(int));

  // Initialize host struct
  for(int i = 0; i < ex->size; i++)
  {
    ex->x[i] = (float)i;
    ex->idx[i] = i;
  }

  // Allocate device struct and copy values from host to device
  Example *d_ex = createDeviceExample(ex);

  printf("\nDevice (ExplicitMem):\n");
  hipLaunchKernelGGL(hipKernel, BLOCKSIZE, BLOCKSIZE, 0, 0, d_ex);
  hipStreamSynchronize(0);
  

  // Free device struct
  freeDeviceExample(d_ex);

  // Free host struct
  free(ex->x);
  free(ex->idx);
  free(ex);
}

/* The main function */
int main(int argc, char* argv[])
{

  

  clock_t tStart = clock();
  runHost();
  clock_t tStop = clock();
  printf("Time %.6fs ", (double)(tStop - tStart)/CLOCKS_PER_SEC);

  tStart = clock();
  runDeviceUnifiedMem();
  tStop = clock();
  printf("Time %.6fs ", (double)(tStop - tStart)/ CLOCKS_PER_SEC);
  
  tStart = clock();
  runDeviceExplicitMem();
  tStop = clock();
  printf("Time %.6fs \n", (double)(tStop - tStart)/ CLOCKS_PER_SEC);
}
