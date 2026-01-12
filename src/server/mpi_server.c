#include <stdio.h>
#include <mpi.h>
#include "common.h"
#include "compute.h"
#include "server_threads.h"
#include "mpi_protocol.h"

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    job_packet_t job;
    result_packet_t result;

    if (rank == 0) {
        while (1) {
            /* Receive job from server */
            MPI_Recv(&job, sizeof(job), MPI_BYTE,
                     MPI_ANY_SOURCE, MPI_JOB_TAG,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            /* Broadcast job to workers */
            MPI_Bcast(&job, sizeof(job), MPI_BYTE, 0, MPI_COMM_WORLD);

            double sum = 0.0;
            for (int i = 1; i < size; i++) {
                double partial;
                MPI_Recv(&partial, 1, MPI_DOUBLE,
                         i, MPI_RESULT_TAG,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                sum += partial;
            }

            result.job_id = job.job_id;
            result.result = sum;

            /* Send result back */
            MPI_Send(&result, sizeof(result), MPI_BYTE,
                     MPI_ANY_SOURCE, MPI_RESULT_TAG,
                     MPI_COMM_WORLD);
        }
    } else {
        while (1) {
            MPI_Bcast(&job, sizeof(job), MPI_BYTE, 0, MPI_COMM_WORLD);
            double r = run_compute(&job);
            MPI_Send(&r, 1, MPI_DOUBLE, 0, MPI_RESULT_TAG, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
