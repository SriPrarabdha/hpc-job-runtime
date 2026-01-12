#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "queue.h"

/*
 * Initialize queue
 */
void queue_init(job_queue_t *q)
{
    q->head = NULL;
    q->tail = NULL;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

/*
 * Destroy queue and free remaining nodes
 */
void queue_destroy(job_queue_t *q)
{
    pthread_mutex_lock(&q->mutex);

    job_node_t *cur = q->head;
    while (cur) {
        job_node_t *tmp = cur;
        cur = cur->next;
        free(tmp);
    }

    q->head = q->tail = NULL;

    pthread_mutex_unlock(&q->mutex);

    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

/*
 * Push job into queue (producer)
 */
void queue_push(job_queue_t *q, job_packet_t *job)
{
    job_node_t *node = (job_node_t *)malloc(sizeof(job_node_t));
    if (!node) {
        perror("queue_push malloc");
        return;
    }

    memcpy(&node->job, job, sizeof(job_packet_t));
    node->next = NULL;

    pthread_mutex_lock(&q->mutex);

    if (q->tail)
        q->tail->next = node;
    else
        q->head = node;

    q->tail = node;

    /* Wake up one waiting consumer */
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

/*
 * Pop job from queue (consumer)
 * Blocks until a job is available
 */
int queue_pop(job_queue_t *q, job_packet_t *job)
{
    pthread_mutex_lock(&q->mutex);

    while (q->head == NULL) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    job_node_t *node = q->head;
    q->head = node->next;
    if (q->head == NULL)
        q->tail = NULL;

    memcpy(job, &node->job, sizeof(job_packet_t));
    free(node);

    pthread_mutex_unlock(&q->mutex);
    return 0;
}
