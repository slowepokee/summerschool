#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

int main(int argc, char **argv){

    int my_id, omp_rank, tag, rec_rank;
    int provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    
    #pragma omp parallel private(omp_rank, tag, rec_rank)
    {
        omp_rank = omp_get_thread_num();
        tag = omp_rank;

        
        
        if(my_id == 0){
            MPI_Send(&omp_rank, 1, MPI_INT, 1,  tag, MPI_COMM_WORLD);
        } else {

            MPI_Recv(&rec_rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("I'm thread %d in process %d, I got %d\n", omp_rank, my_id, rec_rank);
        }
        
        /*
        tag = omp_rank + 500;
    
        MPI_Sendrecv(&omp_rank, 1, MPI_INT, 2, tag, &rec_rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        */
        #pragma omp barrier
        
    }
    
    MPI_Finalize();



    return 0;
}