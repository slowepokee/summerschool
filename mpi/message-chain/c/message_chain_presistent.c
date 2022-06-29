#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void print_ordered(double t);

int main(int argc, char *argv[])
{
    int i, myid, ntasks;
    int msgsize = 100000;
    int *message;
    int *receiveBuffer;
    MPI_Status status;

    double t0, t1;

    int source, destination;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Request req[2];

    /* Allocate message buffers */
    message = (int *)malloc(sizeof(int) * msgsize);
    receiveBuffer = (int *)malloc(sizeof(int) * msgsize);
    /* Initialize message */
    for (i = 0; i < msgsize; i++) {
        message[i] = myid;
    }

    // TODO: set source and destination ranks 
    // Treat boundaries with MPI_PROC_NULL

    MPI_Comm comm_cart;
    int period[2] = {1, 1};
    MPI_Cart_create(MPI_COMM_WORLD, 1, &ntasks, period, 0, &comm_cart);

    
    MPI_Cart_shift(comm_cart, 0, 1, &source, &destination);
    //printf("%i", myid);
    // end TODO


    /* Start measuring the time spent in communication */
    MPI_Barrier(MPI_COMM_WORLD);
    t0 = MPI_Wtime();
    //printf("%d, %d", destination, source);
    // TODO: Send messages 
    
    // MPI_Sendrecv(message, msgsize, MPI_INT, destination, myid+1,
    //              receiveBuffer, msgsize, MPI_INT, source, MPI_ANY_TAG,
    //              MPI_COMM_WORLD, &status);
    
    MPI_Recv_init(receiveBuffer, msgsize, MPI_INT, source, MPI_ANY_TAG, comm_cart, &req[0]);
    MPI_Send_init(message, msgsize, MPI_INT, destination, myid, comm_cart, &req[1]);
    
    MPI_Startall(2, req);

    MPI_Waitall(2, req, MPI_STATUS_IGNORE);


    printf("Sender: %d. Sent elements: %d. Tag: %d. Receiver: %d\n",
           myid, msgsize, myid + 1, destination);

    // TODO: Receive messages
    //MPI_Recv(receiveBuffer, msgsize, MPI_INT, source, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    MPI_Barrier(MPI_COMM_WORLD);
    printf("Receiver: %d. first element %d.\n",
           myid, receiveBuffer[0]);

    // Finalize measuring the time and print it out
    t1 = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    fflush(stdout);

    print_ordered(t1 - t0);

    free(message);
    free(receiveBuffer);
    MPI_Finalize();
    return 0;
}

void print_ordered(double t)
{
    int i, rank, ntasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &ntasks);

    if (rank == 0) {
        printf("Time elapsed in rank %2d: %6.3f\n", rank, t);
        for (i = 1; i < ntasks; i++) {
            MPI_Recv(&t, 1, MPI_DOUBLE, i, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Time elapsed in rank %2d: %6.3f\n", i, t);
        }
    } else {
        MPI_Send(&t, 1, MPI_DOUBLE, 0, 11, MPI_COMM_WORLD);
    }
}
