#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mpi.h>

#define DATASIZE   64
#define WRITER_ID   0

void single_writer(int, int *, int);


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

    single_writer(my_id, localvector, localsize);

    free(localvector);

    MPI_Finalize();
    return 0;
}

void single_writer(int my_id, int *localvector, int localsize)
{
    FILE *fp;
    int *fullvector;
    int i;

    /* TODO: Implement a function that will write the data to file so that
       a single process does the file io. Use rank WRITER_ID as the io rank */

    fullvector = (int *) malloc(DATASIZE * sizeof(int));

    //gather the vectors from all to 0, writen in fullvector (which need to bee datasize*int size)
    MPI_Gather(localvector, localsize, MPI_INT, fullvector, localsize,
               MPI_INT, WRITER_ID, MPI_COMM_WORLD);

    if(my_id == 0){
        //printf("%d", fullvector[10]);
        //write to file
        fp = fopen("test_data", "w");
        fwrite(fullvector, sizeof(int), DATASIZE, fp);
        fclose(fp);
    }


    free(fullvector);
}

