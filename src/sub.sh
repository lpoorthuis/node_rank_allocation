#!/bin/bash -x
#SBATCH --nodes=2
#SBATCH --ntasks=96
#SBATCH --ntasks-per-node=48
#SBATCH --output=job.out
#SBATCH --error=job.err
#SBATCH --time=00:01:00

srun node_rank_allocation.out
