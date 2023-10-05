#include <stdio.h>
#include <mpi.h>

// NOTE: you must define N_PROCESS macro constant in compile time

static int send_count_to[N_PROCESS] = {0};
static double send_time_to[N_PROCESS] = {0.0};

int Comm_translate_rank_to_world(MPI_Comm comm, int rank, int *world_rank)
{
    int result;
    MPI_Group group_world, group_local;

    result = MPI_Comm_group(MPI_COMM_WORLD, &group_world);
    if (result != MPI_SUCCESS) return result;
    result = MPI_Comm_group(comm, &group_local);
    if (result != MPI_SUCCESS) return result;

    result = MPI_Group_translate_ranks(
        group_local, 1, &rank, group_world, world_rank);
    return result;
}

int MPI_Finalize()
{
    int result;

    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if (world_rank == 0) {
        printf("function,src_rank,dest_rank,call_count,total_time\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    result = PMPI_Finalize();

    for (int dest_rank = 0; dest_rank < world_size; dest_rank++) {
        printf(
            "MPI_Send,%d,%d,%d,%f\n",
            world_rank, dest_rank, send_count_to[dest_rank],
            send_time_to[dest_rank]);
    }

    return result;
}

int MPI_Send(const void *buf, int count, MPI_Datatype type, int dest,
              int tag, MPI_Comm comm)
{
    int result;
    int dest_world_rank;

    Comm_translate_rank_to_world(comm, dest, &dest_world_rank);
    double start = MPI_Wtime();
    result = PMPI_Send(buf, count, type, dest, tag, comm);
    send_time_to[dest_world_rank] += MPI_Wtime() - start;
    send_count_to[dest_world_rank]++;

    return result;
}
