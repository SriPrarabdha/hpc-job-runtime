#!/bin/bash
#SBATCH --job-name=hpc_mpi_server
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --time=00:20:00
#SBATCH --output=mpi_server_%j.out

# Load MPI module (adjust if needed)
module load mpi

echo "MPI job starting"
echo "Nodes allocated: $SLURM_JOB_NUM_NODES"
echo "Total tasks: $SLURM_NTASKS"

# One MPI rank per node
srun ./build/mpi_server
