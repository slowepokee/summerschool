#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int rank;
    int array[8][8];
    //TODO: Declare a variable storing the MPI datatype
    MPI_Datatype subtype;

    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Initialize arrays
    if (rank == 0) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                array[i][j] = (i + 1) * 10 + j + 1;
            }
        }
    } else {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                array[i][j] = 0;
            }
        }
    }

    if (rank == 0) {
        printf("Data in rank 0\n");
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                printf("%3d", array[i][j]);
            }
            printf("\n");
        }
    }

    //TODO: Create datatype 
    int sub_size[2] = {4,4};
    int sub_disp[2] = {2,2};
    int arr_size[2] = {8,8};
    MPI_Type_create_subarray(2, arr_size, sub_size, sub_disp, MPI_ORDER_C, MPI_INT, &subtype);
    
    MPI_Type_commit(&subtype);
    //TODO: Send data

    if(rank == 1){
        MPI_Recv(array, 1, subtype, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else if (rank == 0){
        MPI_Send(array, 1, subtype, 1, 4, MPI_COMM_WORLD);
    }

    //TODO: Free datatype
    MPI_Type_free(&subtype);

    // Print out the result on rank 1
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 1) {
        printf("Received data\n");
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                printf("%3d", array[i][j]);
            }
            printf("\n");
        }
    }

    MPI_Finalize();

    return 0;
}
