#include <stdio.h>

#define NX 102400

int main(void)
{
    double vecA[NX], vecB[NX], vecC[NX];

    /* Initialization of the vectors */
    for (int i = 0; i < NX; i++) {
        vecA[i] = 1.0 / ((double) (NX - i));
        vecB[i] = vecA[i] * vecA[i];
    }
    double res = 0.0;
    // TODO start: create a data region and offload the two computations
    // so that data is kept in the device between the computations
    #pragma omp target data map(from:vecC,res) map(to:vecA,vecB)
    {

    #pragma omp target teams distribute map(to:vecA,vecB)
    for (int i = 0; i < NX; i++) {
        vecC[i] = vecA[i] + vecB[i];
    }

    

    #pragma omp target teams distribute parallel for map(to:vecC,vecB) reduction(+:res)
    for (int i = 0; i < NX; i++) {
        res += vecC[i] * vecB[i];
    }
    }
    // TODO end

    double sum = 0.0;
    /* Compute the check value */
    for (int i = 0; i < NX; i++) {
        sum += vecC[i];
    }
    printf("Reduction sum: %18.16f\n", sum);
    printf("Dot product: %18.16f\n", res);

    return 0;
}
