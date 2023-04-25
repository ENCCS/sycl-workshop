Setting up your system
======================

Thanks to IT4I_, we will have an allocation on the Karolina supercomputer for the
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

Working on Karolina
-------------------

You should have received your username in the days leading up to the workshop.
If not, please let us know.

The latest official release of hipSYCL_ and suitable versions of CMake are available as modules on
Karolina:

.. code:: console

   $ module load hipSYCL CMake/3.20.1-GCCcore-10.2.0

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

   $ qsub -A DD-22-28 -q qnvidia -l select=1,walltime=01:00:00 -I

where you ask for:

* 1 node for 1 hour (``-l select=1,walltime=01:00:00``)
* the node is in the `GPU partition <https://docs.it4i.cz/general/resources-allocation-policy/#karolina>`_ (``-q qnvidia``)
* a bash interactive prompt once the job starts (``-I``)

In general, it is preferable to queue the job using a submission script, like the following:

.. code:: bash

   #!/usr/bin/env bash

   #PBS -q qnvidia
   #PBS -N test
   #PBS -l select=1
   #PBS -A DD-22-28

   set -o errexit  # Exit the script on any error
   set -o nounset  # Treat any unset variables as an error

   module --quiet purge  # Reset the modules to the system default
   module load hipSYCL/0.9.2-gcccuda-2020b
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

.. _vnc-sessions:

Graphical sessions using VNC
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We will need to use GUI software during the workshop. It is usually enough to
login with SSH and enable X11 forwarding. However, some GUI programs might be
too demanding for certain connections and it is then better to use other
solutions, like virtual network computing (VNC).
VNC is a graphical desktop-sharing system the remote frame buffer (RFB)
protocol. Karolina is set up to allow such connections and this is what we will
use in the workshop.  The `VNC help page
<https://docs.it4i.cz/general/accessing-the-clusters/graphical-user-interface/vnc/>`_
on the Karolina documentation website has an extensive list of examples that you
can follow. Here we will summarize the most salient points for Linux and macOS
users:

* Install a VNC client on your local machine. `TigerVNC <https://tigervnc.org/>`_ is recommended.
* Start a VNC server instance on Karolina.
* Connect to the VNC server through an SSH tunnel.

Here, in detail, how to proceeed:


#. Open **two** terminal windows: one to start the VNC server, the other for the SSH tunnel.
#. In the **first terminal window**:

   * Login to Karolina with your credentials:

     .. code::

        ssh <your-username>@karolina.it4i.cz
   * Set a VNC password:

     .. code::

        vncpasswd
   * Choose an available display number (<=99). The following command will list
     those already in use:

     .. code::

        ps aux | grep Xvnc | sed -rn 's/(\s) .*Xvnc (\:[0-9]+) .*/\1 \2/p'
   * Start the VNC server:

     .. code::

        vncserver :<your-display-number> -geometry 1600x900 -depth 16

     In the output, pay attention to the following line:

     .. code::

        New 'login1.karolina.it4i.cz:61 (it4i-robedr)' desktop is login1.karolina.it4i.cz:61

     The SSH tunnel needs to be to the `login1` node!
#. In the **second terminal window**:

   * Open an SSH tunnel using as port number ``5900 + display number``:

     .. code::

        ssh -TN -f <your-username>@login1.karolina.it4i.cz -L 5961:localhost:5961
   * Open the VNC host viewer:

     .. code::

        vncviewer localhost:5961

To kill the VNC session:

#. In the **second terminal window**, find the PID of the SSH tunnel and kill the process:

   .. code::

      ps aux | grep ssh
      kill <PID>
#. In the **first terminal window**:

   .. code::

      vncserver -kill :61
