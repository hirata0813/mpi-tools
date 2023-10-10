#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N_SEND 12000
#define N_RECV 1000
#define N_STEP 1000

int64_t bench(int64_t *sendbuf, size_t sendsize, int64_t *recvbuf,
              size_t recvsize, int rank, int size);

int main(int argc, char *argv[]) {
    int64_t sendbuf[N_SEND];
    int64_t recvbuf[N_RECV];
    int rank, n_process;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_process);

    for (size_t i = 1; i <= N_STEP; i++) {
        int64_t sum =
            bench(sendbuf, N_SEND, recvbuf, N_RECV, rank, n_process);
        printf("step %ld done\n", i);
    }

    MPI_Finalize();
    return 0;
}

int64_t bench(int64_t *sendbuf, size_t sendsize, int64_t *recvbuf,
              size_t recvsize, int rank, int size) {
    if (rank == 0) {
        for (size_t i = 0; i < sendsize; i++) {
            sendbuf[i] = i;
        }
    }

    MPI_Scatter(sendbuf, recvsize, MPI_INT64_T, recvbuf, recvsize,
                MPI_INT64_T, 0, MPI_COMM_WORLD);

    int64_t local_sum = 0;
    for (size_t i = 0; i < recvsize; i++) {
        local_sum += recvbuf[i];
    }

    int64_t *local_sums = malloc(size * sizeof(int64_t));
    MPI_Gather(&local_sum, 1, MPI_INT64_T, local_sums, 1, MPI_INT64_T,
               0, MPI_COMM_WORLD);

    int64_t sum = 0;
    if (rank == 0) {
        for (size_t i = 0; i < size; i++) {
            sum += local_sums[i];
        }
    }

    return sum;
}
