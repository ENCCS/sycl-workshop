#!/usr/bin/env bash

#PBS -q qnvidia
#PBS -N test
#PBS -l select=1
#PBS -A DD-22-28
#PBS -l walltime=00:10

set -o errexit  # Exit the script on any error
set -o nounset  # Treat any unset variables as an error

module --quiet purge  # Reset the modules to the system default
module load hipSYCL/0.9.2-gcccuda-2020b
module load CMake/3.20.1-GCCcore-10.2.0
module list

cd $PBS_O_WORKDIR
cmake -S$PBS_O_WORKDIR -Bbuild -DHIPSYCL_TARGETS="cuda:sm_80"
cmake --build build
# and we run from the build folder
./build/<executable-name>

exit 0
