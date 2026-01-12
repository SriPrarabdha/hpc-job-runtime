#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "shm.h"

static int shm_fd = -1;
static shm_region_t *shm_ptr = NULL;

/*
 * Create and map shared memory region
 * Used by server (creator)
 */
shm_region_t* shm_create(size_t size)
{
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open (create)");
        return NULL;
    }

    if (ftruncate(shm_fd, size) < 0) {
        perror("ftruncate");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return NULL;
    }

    shm_ptr = mmap(NULL, size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, shm_fd, 0);

    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        return NULL;
    }

    /* Initialize shared region */
    memset(shm_ptr, 0, size);

    return shm_ptr;
}

/*
 * Attach to existing shared memory
 * Used by MPI leader (if on same node)
 */
shm_region_t* shm_attach(size_t size)
{
    shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open (attach)");
        return NULL;
    }

    shm_ptr = mmap(NULL, size,
                   PROT_READ | PROT_WRITE,
                   MAP_SHARED, shm_fd, 0);

    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return NULL;
    }

    return shm_ptr;
}

/*
 * Cleanup shared memory
 */
void shm_destroy()
{
    if (shm_ptr) {
        munmap(shm_ptr, sizeof(shm_region_t));
        shm_ptr = NULL;
    }

    if (shm_fd >= 0) {
        close(shm_fd);
        shm_fd = -1;
    }

    /* Only creator should unlink */
    shm_unlink(SHM_NAME);
}
