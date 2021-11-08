Setting up your system
======================

Thanks to IZUM_, we will have an allocation on the Vega supercomputer for the
whole duration of the workshop.
The required software stack is already installed and available through the
module system on the supercomputer.

If you prefer to install the required software locally, we refer you to the
`official hipSYCL installation instructions
<https://github.com/illuhad/hipSYCL/tree/develop/install/scripts#readme>`_.

.. warning::

   Your mileage may vary with a local installation: building some of the
   dependencies is an *extremely* time consuming process.  We will **not**
   provide support for local installations during the workshop.

How to follow along
-------------------

This training material, including all exercises and solutions, is available on
`GitHub <https://github.com/ENCCS/sycl-workshop>`_.  To get the most out of it,
we suggest that you clone it locally:

.. code:: console

   $ git clone https://github.com/ENCCS/sycl-workshop

The source code for all exercises and typealongs will then be in the
``content/code`` folder.

Working on Vega
---------------

You should have received your username in the days leading up to the workshop.
If not, please let us know.

The latest official release of hipSYCL_ and CMake are available as modules on
Vega:

.. code:: console

   $ module load hipSYCL CMake

You can compile on the login node with:

.. code:: console

   $ syclcc -o sycl_vadd sycl_vadd.cpp -O3 --hipsycl-targets="omp;cuda:sm_80"

However, it will be more convenient to use the CMake scripts we provide with the
source files:

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

.. code:: bash

   #!/usr/bin/env bash

   #SBATCH --partition=gpu
   #SBATCH --job-name=test
   #SBATCH --mem=4G
   #SBATCH --reservation=sycl
   #SBATCH --gres=gpu:1
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
