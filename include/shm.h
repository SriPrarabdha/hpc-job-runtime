#ifndef SMH_H
#define SMH_H

#include <stddef.h>
#include "common.h"

typedef struct {
    job_packet_t job;
    result_packet_t result;
    int job_ready;
    int result_ready;
} shm_region_t;


shm_region_t* shm_create(size_t size);
shm_region_t* shm_attach(size_t size);

void shm_destroy();

#endif