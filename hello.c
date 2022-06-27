#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]){
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	

	
	printf("Hello from %d\n", rank);
	
	MPI_Barrier(MPI_COMM_WORLD);
	if(rank == 0){
	
 	printf("Size %d\n", size);
	}

	MPI_Finalize();
}
