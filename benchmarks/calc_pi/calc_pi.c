#include <bits/time.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>

#define N 100000000000 // max: 18,446,744,073,709,551,615

double nsec_to_ms(time_t nsec) {
    return (double)nsec / (1000*1000*1000);
}

double timespec_to_sec(time_t sec, time_t nsec) {
    return (double)sec + nsec_to_ms(nsec);
}

double calc_elapsed_time(struct timespec start, struct timespec end) {
    return timespec_to_sec(end.tv_sec - start.tv_sec, end.tv_nsec - start.tv_nsec);
}

int main(int argc, char **argv)
{
    unsigned long i, loop_start, loop_end;
    int rank, size;
    double x, pi;
    double res = 0.0;
    struct timespec ct1, ct2, ct3, ct4, ct5, ct6;
    struct timespec rt1, rt2, rt3, rt4, rt5, rt6;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct1);
    clock_gettime(CLOCK_REALTIME, &rt1);
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    loop_start = (N / size) * rank;
    loop_end = (N / size) * (rank + 1);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct2);
    clock_gettime(CLOCK_REALTIME, &rt2);
    MPI_Barrier(MPI_COMM_WORLD);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct3);
    clock_gettime(CLOCK_REALTIME, &rt3);

    for(i = loop_start; i < loop_end; i++){
        x = (i + 0.5) / N;
        res += 4 / (1 + x*x);
    }

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct4);
    clock_gettime(CLOCK_REALTIME, &rt4);
    MPI_Reduce(&res, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct5);
    clock_gettime(CLOCK_REALTIME, &rt5);
    MPI_Finalize();
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ct6);
    clock_gettime(CLOCK_REALTIME, &rt6);
    pi = pi / N;
    if (rank == 0) {
        printf("PI = %.50lf\n", pi);
        printf("\nStatistics: Real Time (sec) | CPU Time (sec)\n");
        printf("T1: %f | %f\n", timespec_to_sec(rt1.tv_sec, rt1.tv_nsec), timespec_to_sec(ct1.tv_sec, ct1.tv_nsec));
        printf("T6: %f | %f\n", timespec_to_sec(rt6.tv_sec, rt6.tv_nsec), timespec_to_sec(ct6.tv_sec, ct6.tv_nsec));
        printf("Total Time:             %f | %f\n", calc_elapsed_time(rt1, rt6), calc_elapsed_time(ct1, ct6));
        printf("T2 - T1 (MPI_Init):     %f | %f\n", calc_elapsed_time(rt1, rt2), calc_elapsed_time(ct1, ct2));
        printf("T3 - T2 (MPI_Barrier):  %f | %f\n", calc_elapsed_time(rt2, rt3), calc_elapsed_time(ct2, ct3));
        printf("T4 - T3 (for loop):     %f | %f\n", calc_elapsed_time(rt3, rt4), calc_elapsed_time(ct3, ct4));
        printf("T5 - T4 (MPI_Reduce):   %f | %f\n", calc_elapsed_time(rt4, rt5), calc_elapsed_time(ct4, ct5));
        printf("T6 - T5 (MPI_Finalize): %f | %f\n", calc_elapsed_time(rt5, rt6), calc_elapsed_time(ct5, ct6));
    }

    return 0;
}

