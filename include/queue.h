#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "common.h"

typedef struct job_node{
    job_packet_t job;
    struct job_node *next;
} job_node_t;

typedef struct {
    job_node_t *head;
    job_node_t *tail;
    pthread_mutex_t mutex;
    pthread_mutex_t cond;
} job_queue_t ;

void queue_init(job_queue_t *q);
void queue_destroy(job_queue_t *q);

void queue_push(job_queue_t *q , job_packet_t *job);
void queue_pop(job_queue_t *q , job_packet_t *job);


#endif