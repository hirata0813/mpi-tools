#include <mpi.h>
#include <unistd.h>

#define N_STEP 20000000000

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    for (size_t i = 0; i < N_STEP; i++) {
        asm volatile("nop");
    }

    MPI_Finalize();
    return 0;
}
