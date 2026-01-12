#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "socket_utils.h"
#include "common.h"

#define _POSIX_C_SOURCE 200809L


#define DEFAULT_NUM_JOBS 5

static void fill_job(job_packet_t *job, int job_id)
{
    job->job_id = job_id;
    job->data_len = MAX_DATA_SIZE;

    for (int i = 0; i < job->data_len; i++) {
        job->data[i] = (double)(i % 100) * 0.01;
    }
}

static double time_diff_sec(struct timespec a, struct timespec b)
{
    return (b.tv_sec - a.tv_sec) +
           (b.tv_nsec - a.tv_nsec) * 1e-9;
}

int main(int argc, char **argv)
{
    const char *server_ip = "127.0.0.1";
    int port = SERVER_PORT;
    int num_jobs = DEFAULT_NUM_JOBS;

    if (argc >= 2)
        server_ip = argv[1];
    if (argc >= 3)
        num_jobs = atoi(argv[2]);

    printf("Client starting\n");
    printf("Server: %s:%d\n", server_ip, port);
    printf("Jobs: %d\n\n", num_jobs);

    for (int i = 0; i < num_jobs; i++) {
        int sockfd = connect_to_server(server_ip, port);
        if (sockfd < 0) {
            fprintf(stderr, "Failed to connect to server\n");
            continue;
        }

        job_packet_t job;
        result_packet_t result;

        fill_job(&job, i);

        struct timespec t_start, t_end;
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        if (send_job(sockfd, &job) != sizeof(job)) {
            perror("send_job");
            close(sockfd);
            continue;
        }

        if (recv_result(sockfd, &result) != sizeof(result)) {
            perror("recv_result");
            close(sockfd);
            continue;
        }

        clock_gettime(CLOCK_MONOTONIC, &t_end);

        printf("Job %d completed | result = %.6f | latency = %.3f ms\n",
               result.job_id,
               result.result,
               time_diff_sec(t_start, t_end) * 1000.0);

        close(sockfd);
    }

    printf("\nClient done\n");
    return 0;
}
