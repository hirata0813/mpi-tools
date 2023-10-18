#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N_DATA 160000
#define N_EACH 10000
#define N_STEP 10000

int main(int argc, char *argv[]) {
    int64_t sendbuf[N_DATA];
    int64_t recvbuf[N_DATA];

    MPI_Init(&argc, &argv);

    for (size_t i = 0; i < N_DATA; i++) {
        sendbuf[i] = i;
    }
    for (size_t i = 1; i <= N_STEP; i++) {
        MPI_Alltoall(sendbuf, N_EACH, MPI_INT64_T, recvbuf, N_EACH,
                     MPI_INT64_T, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
