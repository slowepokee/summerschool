#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mpi.h>

#define DATASIZE   64
#define WRITER_ID   0

void mpiio_writer(int, int *, int);
void mpiio_reader(int, int *, int);
void ordered_print(int, int, int *, int);

int main(int argc, char *argv[])
{
    int my_id, ntasks, i, localsize;
    int *localvector;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    if (ntasks > 64) {
        fprintf(stderr, "Datasize (64) should be divisible by number "
                "of tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    if (DATASIZE % ntasks != 0) {
        fprintf(stderr, "Datasize (64) should be divisible by number "
                "of tasks.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    localsize = DATASIZE / ntasks;
    localvector = (int *) malloc(localsize * sizeof(int));

    for (i = 0; i < localsize; i++) {
        localvector[i] = i + 1 + localsize * my_id;
    }

    mpiio_reader(my_id, localvector, localsize);

    ordered_print(ntasks, my_id, localvector, localsize);

    free(localvector);

    MPI_Finalize();
    return 0;
}

void mpiio_writer(int my_id, int *localvector, int localsize)
{
    MPI_File fh;
    MPI_Offset offset;

    //offset for each process
    offset = sizeof(int)*localsize*my_id;


    MPI_File_open(MPI_COMM_WORLD, "test_data", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &fh);


    MPI_File_write_at_all(fh, offset, localvector, localsize, MPI_INT, MPI_STATUS_IGNORE);

    MPI_File_close(&fh);


}

void mpiio_reader(int my_id, int *localvector, int localsize)
{
    MPI_File fh;
    MPI_Offset offset;

    //offset for each process
    offset = sizeof(int)*localsize*my_id;


    MPI_File_open(MPI_COMM_WORLD, "test_data", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

    //then read from all, with the given offsets
    MPI_File_read_at_all(fh, offset, localvector, localsize, MPI_INT, MPI_STATUS_IGNORE);

    MPI_File_close(&fh);


}

void ordered_print(int ntasks, int rank, int *buffer, int n)
{
    int task, i;

    for (task = 0; task < ntasks; task++) {
        if (rank == task) {
            printf("Task %i received:", rank);
            for (i = 0; i < n; i++) {
                printf(" %2i", buffer[i]);
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}


