#include "mpi.h"
#include "stdio.h"

int main(int argc, char **argv)
{
    int myrank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
    printf("Hello from %d node\n",myrank);
    MPI_Finalize();
}