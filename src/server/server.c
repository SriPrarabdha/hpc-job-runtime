#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "queue.h"
#include "shm.h"
#include "socket_utils.h"
#include "server_threads.h"
#include "common.h"
#include "mpi_protocol.h"

int main()
{
    job_queue_t queue;
    queue_init(&queue);

    shm_region_t *shm = shm_create(sizeof(shm_region_t));
    if (!shm) {
        fprintf(stderr, "Failed to create shared memory\n");
        exit(1);
    }

    int server_fd = create_server_socket(SERVER_PORT);
    if (server_fd < 0) {
        fprintf(stderr, "Failed to create server socket\n");
        exit(1);
    }

    pthread_t user_thread, mpi_thread;

    user_thread_args_t uargs = {
        .server_socket = server_fd,
        .queue = &queue
    };

    mpi_thread_args_t margs = {
        .queue = &queue,
        .shm = shm
    };

    pthread_create(&user_thread, NULL, user_thread_fn, &uargs);
    pthread_create(&mpi_thread, NULL, mpi_thread_fn, &margs);

    pthread_join(user_thread, NULL);
    pthread_join(mpi_thread, NULL);

    shm_destroy();
    queue_destroy(&queue);
    close(server_fd);

    return 0;
}
