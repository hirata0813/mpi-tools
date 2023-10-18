#!/bin/sh

#SBATCH -n 16
#SBATCH -J forever-alltoall

srun main
