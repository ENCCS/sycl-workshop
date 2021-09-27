Setting up your system
======================

.. todo::

   - How to get an account of Vega.
   - How to install hipSYCL locally.


Working on Vega
---------------

The latest official release of hipSYCL_ is available as a module on Vega:

.. code:: console

   $ module load hipSYCL

You can compile on the login node with:

.. code:: console

   $ syclcc -o sycl_vadd sycl_vadd.cpp -O3 --hipsycl-targets="omp;cuda:sm_80"

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

.. code:: text

   #!/usr/bin/env bash

   #SBATCH --partition=gpu
   #SBATCH --job-name=test
   #SBATCH --mem=4G
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

   exit 0
