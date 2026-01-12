cmake_minimum_required(VERSION 3.16)
project(hpc_demo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(MPI REQUIRED)

find_package(Threads REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

# Server
add_executable(server
    src/server/server.c
    src/server/user_thread.c
    src/server/mpi_thread.c
    src/server/socket_utils.c
    src/server/queue.c
    src/server/shm.c
)

target_link_libraries(server
    MPI::MPI_C
    Threads::Threads
    rt
)

# MPI server
add_executable(mpi_server
    src/mpi/mpi_server.c
    src/compute/compute.c
)

target_link_libraries(mpi_server
    MPI::MPI_C
    m
)

# client
add_executable(client
    src/client/client.c
    src/server/socket_utils.c
)

target_link_libraries(client
    Threads::Threads
)
