#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <netinet/in.h>
#include "common.h"

int create_server_socket(int port);
int accept_client(int server_fd);

int connect_to_server(const char *host , int port);

int send_job(int sockfd , const job_packet_t *job);
int recv_result(int sockfd , result_packet_t *result);

#endif