#ifndef SERVER_THREADS_H
#define SERVER_THREADS_H

#include "queue.h"
#include "shm.h"

typedef struct {
    job_queue_t *queue;
    int server_socket;
} user_thread_args_t;

typedef struct {
    job_queue_t *queue;
    shm_region_t *shm;
} mpi_thread_args_t;

void* user_thread_fn(void *args);

void* mpi_thread_fn(void *args);

#endif