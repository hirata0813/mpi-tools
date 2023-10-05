#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OBSR_RANK 0
/* random integer */
#define RARELY_CONFLICT_TAG 1795111794

void send_event(char *funcname, char *eventname, int rank) {
    char buf[32];

    sprintf(buf, "%s,%s,%d", funcname, eventname, rank);
    // send buf including terminating null char
    MPI_Send(buf, 32, MPI_CHAR, OBSR_RANK, RARELY_CONFLICT_TAG,
             MPI_COMM_WORLD);
}

void *run_observer(void *) {
    char buf[32];
    MPI_Status status;
    int n_process_waiting = 0;

    char funcname[32], eventname[32];
    int rank;

    while (1) {
        int initialized, finalized;
        MPI_Initialized(&initialized);
        MPI_Finalized(&finalized);
        if (!initialized || finalized) {
            break;
        }
        MPI_Recv(buf, 32, MPI_CHAR, MPI_ANY_SOURCE, RARELY_CONFLICT_TAG,
                 MPI_COMM_WORLD, &status);
        sscanf(buf, "%[^,],%[^,],%d", funcname, eventname, &rank);
        if (strcmp(eventname, "begin") == 0) {
            n_process_waiting += 1;
        } else if (strcmp(eventname, "end") == 0) {
            n_process_waiting -= 1;
        } else {
            // unreachable
        }
#ifdef DEBUG
        printf("%s,%s,%d,%d\n", funcname, eventname, rank,
               n_process_waiting);
#endif
    }

    return NULL;
}

int MPI_Init(int *argc, char ***argv) {
    int result;
    int provided, required;

    required = MPI_THREAD_MULTIPLE;
    result = MPI_Init_thread(argc, argv, required, &provided);
    if (provided < required) {
        fprintf(stderr,
                "TRACER ERROR: MPI_THREAD_MULTIPLE (%d) required, but "
                "provided %d\n",
                required, provided);
        return MPI_ERR_UNKNOWN;
    }
    return result;
}

int MPI_Init_thread(int *argc, char ***argv, int required,
                    int *provided) {
    int result, rank, size;
    pthread_t observer_thread_handle;

    result = PMPI_Init_thread(argc, argv, required, provided);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == OBSR_RANK) {
        pthread_create(&observer_thread_handle, NULL, run_observer,
                       NULL);
    }

    return result;
}

int MPI_Scatter(const void *sendbuf, int sendcount,
                MPI_Datatype sendtype, void *recvbuf, int recvcount,
                MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int result, world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    send_event("MPI_Scatter", "begin", world_rank);
    result = PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf,
                          recvcount, recvtype, root, comm);
    send_event("MPI_Scatter", "end", world_rank);

    return result;
}

int MPI_Gather(const void *sendbuf, int sendcount,
               MPI_Datatype sendtype, void *recvbuf, int recvcount,
               MPI_Datatype recvtype, int root, MPI_Comm comm) {
    int result, world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    send_event("MPI_Gather", "begin", world_rank);
    result = PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf,
                         recvcount, recvtype, root, comm);
    send_event("MPI_Gather", "end", world_rank);

    return result;
}
