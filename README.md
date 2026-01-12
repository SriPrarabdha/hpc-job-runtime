<!--

 gcloud compute ssh mpplabc34n-slurm-login-001 --zone=asia-south1-b --project=mpplab-482405

mkdir -p build
cd build
cmake ..
make -j
cd ..


srun --mpi=pmi2 --nodes=2 --ntasks-per-node=1 ./build/mpi_server

squeue

scancel 82

 -->
> [!IMPORTANT]
> A client submits a compute request, the server queues it using POSIX shared memory and pthreads, dispatches it to an MPI master–worker pool running under Slurm, aggregates the results, and returns them to the client.

 ## High Level Flow

-  A client generates fake compute jobs and sends them to the server over TCP.

- The server runs on the head node and:

    1.accepts client requests,
    2. queues jobs using a thread-safe queue,
    3. dispatches jobs to an MPI program.

- The MPI program, launched via Slurm:

    1. uses a master–worker model,
    2. distributes work across available nodes, aggregates results.

- Results are returned to the server and then back to the client.

```
+------------------+        TCP Socket        +---------------------------+
|     CLIENT       |  -------------------->  |           SERVER          |
|                  |                          |  (Head Node, LP Runtime) |
| - Generates jobs |                          |                           |
| - Sends packets  |                          |  +---------------------+ |
| - Receives result|                          |  |  User Thread        | |
+------------------+                          |  |  (socket accept)    | |
                                              |  +----------+----------+ |
                                              |             |            |
                                              |        enqueue(job)      |
                                              |             v            |
                                              |  +---------------------+ |
                                              |  | Thread-safe Queue   | |
                                              |  | (mutex + condvar)   | |
                                              |  +----------+----------+ |
                                              |             |            |
                                              |        dequeue(job)      |
                                              |             v            |
                                              |  +---------------------+ |
                                              |  | MPI Thread          | |
                                              |  | (dispatcher)        | |
                                              |  +----------+----------+ |
                                              |             |            |
                                              |     POSIX Shared Memory  |
                                              |   (shm_open + mmap)     |
                                              +-------------+------------+
                                                            |
                                                            | MPI (Slurm)
                                                            v
                   +-------------------------------------------------------------------+
                   |                         MPI PROGRAM                               |
                   |                  (Launched via Slurm / srun)                      |
                   |                                                                   |
                   |  Rank 0 (Leader)                                                  |
                   |  ----------------                                                 |
                   |  - Receives job from server                                       |
                   |  - Broadcasts job                                                 |
                   |  - Aggregates results                                             |
                   |  - Sends result back                                              |
                   |                                                                   |
                   |  Ranks 1..N (Workers)                                             |
                   |  --------------------                                             |
                   |  - Receive job                                                    |
                   |  - Run compute kernel                                             |
                   |  - Send partial result                                            |
                   +-------------------------------------------------------------------+
```

## Important Components

1. src/server/server.c

- Initializes shared memory and job queue
- Starts two threads:

    - User thread (receives jobs)
    - MPI thread (dispatches jobs)

2. src/server/user_thread.c

    - Accepts TCP connections
    - Receives jobs
    - Pushes them into a queue

3. src/server/queue.c

    - Producer–consumer pattern
    - Mutex + condition variable

4. src/server/mpi_thread

    - Pulls jobs from queue
    - Writes to shared memory
    - Sends jobs to MPI leader
    - Receives aggregated results
