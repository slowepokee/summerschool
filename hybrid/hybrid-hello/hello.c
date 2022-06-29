#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char **argv){

    int my_id, omp_rank;
    int num_threads, thread_num;
    
    #pragma omp parallel
    {   
        num_threads = omp_get_num_threads();
        thread_num = omp_get_thread_num();
        
        printf("Hello from %d of %d!\n", thread_num, num_threads);
    }
    


    return 0;
}