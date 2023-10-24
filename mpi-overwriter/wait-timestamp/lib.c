#include <mpi.h>
#include <time.h>
#include <stdio.h>

#define SEC2NSEC 1000000000

static inline void log_timestamp(char *funcname, char *eventname,
                                 int rank, struct timespec ts)
{
    long int nanosec = ts.tv_sec * SEC2NSEC + ts.tv_nsec;
    printf("%s,%s,%d,%ld\n", funcname, eventname, rank, nanosec);
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
    int result, world_rank;
    struct timespec start, end;

    timespec_get(&start, TIME_UTC);
    result = PMPI_Wait(request, status);
    timespec_get(&end, TIME_UTC);

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    log_timestamp("MPI_Wait", "before", world_rank, start);
    log_timestamp("MPI_Wait", "after", world_rank, end);

    return result;
}

int MPI_Waitall(int count, MPI_Request array_of_requests[],
                MPI_Status *array_of_statuses)
{
    int result, world_rank;
    struct timespec start, end;

    timespec_get(&start, TIME_UTC);
    result = PMPI_Waitall(count, array_of_requests, array_of_statuses);
    timespec_get(&end, TIME_UTC);

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    log_timestamp("MPI_Waitall", "before", world_rank, start);
    log_timestamp("MPI_Waitall", "after", world_rank, end);

    return result;
}
