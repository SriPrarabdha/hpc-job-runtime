#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "server_threads.h"
#include "socket_utils.h"

void* user_thread_fn(void *arg)
{
    user_thread_args_t *args = (user_thread_args_t *)arg;
    int server_fd = args->server_socket;
    job_queue_t *queue = args->queue;

    while (1) {
        int client_fd = accept_client(server_fd);
        if (client_fd < 0)
            continue;

        job_packet_t job;
        ssize_t n = recv(client_fd, &job, sizeof(job), MSG_WAITALL);
        if (n == sizeof(job)) {
            queue_push(queue, &job);
        }

        close(client_fd);
    }
    return NULL;
}
