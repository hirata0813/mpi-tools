#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N_DATA 1000000
#define N_STEP 1000

int64_t communicate_large_data_and_calc(int64_t *data, size_t datasize,
                                        int rank, int size);

int main(int argc, char *argv[]) {
    int64_t data[N_DATA];
    int rank, n_process;

    int provided_thread_support_level;
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_process);

    for (size_t i = 1; i <= N_STEP; i++) {
        int64_t sum =
            communicate_large_data_and_calc(data, N_DATA, rank, n_process);
    }

    MPI_Finalize();
    return 0;
}

int64_t communicate_large_data_and_calc(int64_t *data, size_t datasize,
                                        int rank, int size) {
    if (rank == 0) {
        for (size_t i = 0; i < datasize; i++) {
            data[i] = i;
        }
    }

    MPI_Scatter(data, datasize / size, MPI_INT64_T, data, datasize / size,
                MPI_INT64_T, 0, MPI_COMM_WORLD);

    int64_t local_sum = 0;
    for (size_t i = 0; i < datasize / size; i++) {
        local_sum += data[i];
    }

    int64_t *local_sums = malloc(size * sizeof(int64_t));
    MPI_Gather(&local_sum, 1, MPI_INT64_T, local_sums, 1, MPI_INT64_T, 0,
               MPI_COMM_WORLD);

    int64_t sum = 0;
    if (rank == 0) {
        for (size_t i = 0; i < size; i++) {
            sum += local_sums[i];
        }
    }

    return sum;
}
