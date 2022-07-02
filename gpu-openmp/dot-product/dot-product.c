#include <stdio.h>

#define NX 102400

int main(void)
{
    double vecA[NX], vecB[NX];
    double test = 2.0;

    // Initialization of the vectors
    for (int i = 0; i < NX; i++) {
        vecA[i] = 1.0 / ((double) (NX - i));
        vecB[i] = vecA[i] * vecA[i];
    }

    // TODO start: offload and parallelize the computation
    
    double res = 0.0;
    #pragma omp target map(to:vecA,vecB) 
    #pragma omp teams distribute parallel for reduction(+:res) reduction(+:test) 
    for (int i = 0; i < NX; i++) {
        res +=  vecA[i] * vecB[i];
        test += vecA[i] + vecB[i];
    }
    
    // TODO end

    printf("Dot product: %18.16f\n", res);
    printf("Test sum: %18.16f\n", test);

    return 0;
}
