#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket_utils.h"

/* ---------- Server-side ---------- */

int create_server_socket(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }

    if (listen(fd, 8) < 0) {
        perror("listen");
        close(fd);
        return -1;
    }

    return fd;
}

int accept_client(int server_fd)
{
    return accept(server_fd, NULL, NULL);
}

/* ---------- Client-side ---------- */

int connect_to_server(const char *host, int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &addr.sin_addr);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return -1;
    }

    return fd;
}

/* ---------- Data transfer ---------- */

int send_job(int sockfd, const job_packet_t *job)
{
    return send(sockfd, job, sizeof(*job), 0);
}

int recv_result(int sockfd, result_packet_t *result)
{
    return recv(sockfd, result, sizeof(*result), MSG_WAITALL);
}
