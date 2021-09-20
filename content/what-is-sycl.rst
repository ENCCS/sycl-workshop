.. _what-is-sycl:

What is SYCL?
=============

.. questions::

   - What is SYCL_ and why should I care?

.. objectives::

   - Learn how to write a simple C++ SYCL_ code.
   - Learn how to compile hipSYCL_.
   - Learn about device discovery in SYCL_ code.


The high-performance computing landscape is increasingly dominated by machines
whose high FLOP counts is delivered by *heterogeneous hardware*: large-core
count CPUs in tandem with ever more powerful GPUs are now the norm in the HPC
datacenter.  This trend is likely to continue, with the appearance of new
hardware architectures, sometimes tailored for specific operations.

Each new architecture comes equipped with its own, usually low-level,
programming language. Adapting applications for a heterogeneous computing
environment proceeds in tight cycles of profiling and porting. These are
time-consuming, error-prone, and scarcely portable:

* Mastering low-level programming languages may require years.
* The codebase can *diverge* significantly to work with different hardware.


.. todo::

   Complete intro:

   - Standards-based, vendor-agnostic programming for heterogeneous hardware
   - Compilation model
   - Evolution in relation to C++ standard (Khronos group has a chart, I think)

.. tabs::

   .. tab:: CUDA

      .. literalinclude:: code/snippets/saxpy.cu
         :language: cuda

   .. tab:: HIP

      .. literalinclude:: code/snippets/saxpy.hip.cpp
         :language: c++

   .. tab:: OpenMP

      .. literalinclude:: code/snippets/saxpy.omp.cpp
         :language: c++

   .. tab:: OpenACC

      .. literalinclude:: code/snippets/saxpy.acc.c
         :language: c

Hello, SYCL!
------------

.. todo::

   - Add two vectors example in OpenMP, SYCL, CUDA, and HIP.
   - Highlight queues, command groups, kernels.

.. typealong:: "Hello world" with SYCL

   Download  :download:`scaffold project <code/tarballs/00_hello-cxx.tar.bz2>`.

   .. literalinclude:: code/day-1/00_hello-cxx/hello.cpp
      :language: c++

   You can download the :download:`complete, working example <code/tarballs/00_hello-cxx_solution.tar.bz2>`.

   Then unpack the archive::

     tar xf hello-cxx_solution.tar.bz2



.. keypoints::

   - SYCL is an *open-source standard* to describe parallelism on heterogeneous
     platforms using the C++ programming language.
   - There are many implementations of the SYCL standard: Intel DPC++, hipSYCL,
     CodePlay ComputeCPP.
