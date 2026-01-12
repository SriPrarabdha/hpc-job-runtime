#!/bin/bash
set -e

NODES=${1:-4}
JOBS=${2:-5}

echo "==== HPC Demo Launcher ===="
echo "Nodes: $NODES"
echo "Jobs:  $JOBS"
echo

# Build
mkdir -p build
cd build
cmake ..
make -j
cd ..

# Launch MPI server in background
echo "Launching MPI server..."
sbatch --nodes=$NODES scripts/run_mpi.slurm
sleep 5

# Launch server (head node)
echo "Launching server..."
./build/server &
SERVER_PID=$!
sleep 2

# Launch client
echo "Launching client..."
./build/client 127.0.0.1 $JOBS

# Cleanup
kill $SERVER_PID
echo "Demo complete"
