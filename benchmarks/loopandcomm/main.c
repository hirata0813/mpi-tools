#include <mpi.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

#define NOPSTEP 120000000
#define COMMSTEP 5
#define LOOPNUM 177
#define N_DATA 160000
#define N_EACH 10000

int main(int argc, char *argv[]){
    int64_t sendbuf[N_DATA];
    int64_t recvbuf[N_DATA];

    MPI_Init(&argc, &argv);

    for (size_t l = 0; l < N_DATA; l++) {
        sendbuf[l] = l;
    }

    for (int i = 0; i < LOOPNUM; i++){
        for (int j = 0; j < NOPSTEP; j++){ // nop loop
            asm volatile("nop");
        }
    
        for (int k = 0; k < COMMSTEP; k++){ //comm loop
            MPI_Alltoall(sendbuf, N_EACH, MPI_INT64_T, recvbuf, N_EACH,
                         MPI_INT64_T, MPI_COMM_WORLD);
        }
    }
  
    MPI_Finalize();
    return 0;
}
