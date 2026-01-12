#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#define MAX_DATA_SIZE 1024
#define SHM_NAME "/hpc_demo_shm"
#define SERVER_PORT 9090

typedef struct {
    int job_id;
    int data_len;

    double data[MAX_DATA_SIZE];
} job_packet_t;

typedef struct {
    int job_id;
    double result;
} result_packet_t;

#endif