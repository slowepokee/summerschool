#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#define n 1680


int main(int argc, char** argv)
{
    int myid, ntasks;

    if(myid==0){
        printf("Computing approximation to pi with N=%d\n", n);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    //Assign the partial sums to tasks
    int istart = myid*(n/ntasks) + 1;
    int istop = (myid + 1)*(n/ntasks);

    //Calculate the partial sums
    double pi = 0.0;
    for (int i=istart; i <= istop; i++) {
        double x = (i - 0.5) / n;
        pi += 1.0 / (1.0 + x*x);
    }

    pi *= 4.0 / n;
    double final_pi;
    //printf("%d", ntasks);

    //Reduce sum
    MPI_Reduce(&pi, &final_pi, 1, MPI_DOUBLE,
                MPI_SUM, 0, MPI_COMM_WORLD);


                
    if(myid == 0){
        printf("Approximate pi=%18.16f (exact pi=%10.8f)\n", final_pi, M_PI);
    }


    MPI_Finalize();

}
   