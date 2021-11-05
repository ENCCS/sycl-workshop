Setting up your system
======================

.. todo::

   - How to get an account of Vega.
   - How to install hipSYCL locally.

How to follow along
-------------------

This training material, including all exercises and solutions, is available on GitHub.
To get the most out of it, we suggest that you clone it locally.

.. code:: console

   $ git clone https://github.com/ENCCS/sycl-workshop

The source code will then be in the ``content/code`` folder.

Working on Vega
---------------

The latest official release of hipSYCL_ is available as a module on Vega:

.. code:: console

   $ module load CMake hipSYCL

You can compile on the login node with:

.. code:: console

   $ syclcc -o sycl_vadd sycl_vadd.cpp -O3 --hipsycl-targets="omp;cuda:sm_80"

However, it will be more convenient to use the CMake scripts we provide with the source files:

.. code:: console

   $ cmake -S. -Bbuild -DHIPSYCL_TARGETS="omp;cuda:sm_80"
   $ cmake --build build

You can also request an interactive job with:

.. code:: console

   $ srun --nodes=1 --exclusive --partition=gpu --time=01:00:00 --pty bash -i

where you ask for:

* 1 node (``--nodes=1``) in the `GPU partition <https://doc.vega.izum.si/architecture/#gpu-partition>`_ (``--partition=gpu``) for exclusive use (``--exclusive``)
* a 1-hour long allocation (``--time=01:00:00``)
* a bash interactive prompt once the job starts (``--pty bash -i``)

In general, it is preferable to queue the job using a submission script, like the following:

.. todo::

   - Add info about reservation!

.. code:: bash

   #!/usr/bin/env bash

   #SBATCH --partition=gpu
   #SBATCH --job-name=test
   #SBATCH --mem=4G
   #SBATCH --reservation=sycl
   #SBATCH --time=00-01:00:00
   #SBATCH --output=test_out.log
   #SBATCH --error=test_err.log

   set -o errexit  # Exit the script on any error
   set -o nounset  # Treat any unset variables as an error

   module --quiet purge  # Reset the modules to the system default
   module load hipSYCL/0.9.1-gcccuda-2020b
   module list

   # compile code
   syclcc -o sycl_vadd sycl_vadd.cpp -O3 --hipsycl-targets="cuda:sm_80"

   # run executable
   ./sycl_vadd

   # we can also use CMake to build
   # module load CMake/3.18.4-GCCcore-10.2.0
   # cmake -S$SLURM_SUBMIT_DIR -Bbuild -DHIPSYCL_TARGETS="cuda:sm_80"
   # and we run from the build folder
   # ./build/sycl_vadd

   exit 0
