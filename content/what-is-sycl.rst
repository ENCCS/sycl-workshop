.. _what-is-sycl:

What is SYCL?
=============

.. questions::

   - What is SYCL_ and why should I care?

.. objectives::

   - Learn how to write a simple C++ SYCL_ code.
   - Learn how to compile with hipSYCL_.


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

Let's look at a simple example: the single-precision vector addition kernel
:math:`y := \alpha x + y`, better known as ``SAXPY``:

.. tabs::

   .. tab:: CUDA

      Implementation using the native language and API for Nvidia GPUs. This code is an example of *prescriptive* parallelism.

      .. literalinclude:: code/snippets/saxpy.cu
         :language: cuda

   .. tab:: HIP

      Implementation using the native language and API for AMD GPUs, but also compatible with Nvidia GPUs.
      This code is an example of *prescriptive* parallelism.

      .. literalinclude:: code/snippets/saxpy.hip.cpp
         :language: c++

   .. tab:: OpenMP

      Implementation using standard ``pragma``s.
      This code is an example of *descriptive* parallelism.

      .. literalinclude:: code/snippets/saxpy.omp.cpp
         :language: c++

   .. tab:: OpenACC

      Implementation using standard ``pragma``s.
      This code is an example of *descriptive* parallelism.

      .. literalinclude:: code/snippets/saxpy.acc.c
         :language: c


There are few things to notice in this example:

- We work with *prescriptive* parallelism with CUDA_ and HIP_: we explicitly
  divide up the work between threads inside the kernel function.
- Instead in the OpenMP_ and OpenACC_ models, we describe data movement and
  worksharing, but rely on the compiler to generate correct code. Hence the
  qualification of *descriptive* parallelism.
- When using low-level APIs, a certain degree of code duplication between host
  and device code **be inevitable**. Parallelization strategies between host
  and device will be different.  On top of this, the expressiveness of
  low-level language extensions is limited, which might increase the
  maintenance burden.
- ``pragma``-based schemes are standardized, but compiler support is not always
  optimal.

When programming for heterogenous platforms, there is no silver bullet: we have
to carefully evaluate the tradeoffs between performance, productivity, and
portability.

SYCL_ is an attempt [#f1]_ at improving this state of affairs. It is a
*standards-based* and *vendor-agnostic* domain-specific embedded language for
parallel programming, not necessarily on heterogeneous architectures.
The SYCL_ standard is developed by the `Khronos group <https://www.khronos.org/>`_:

* It is built as a **header-only** library for ISO C++17. SYCL_ code can be
  compiled with a standards-compliant compiler and the necessary headers: it
  does **not** require special compiler extensions.
* It is a **single-source**-style framework. Host and device code are in the
  same *translation unit*.
* It is **asynchronous**. We describe computations and memory operations, the
  runtime generates a task graph and works its way through it when executing the
  program.

The SYCL compiler
-----------------

We will need a SYCL-aware compiler when working with SYCL.  These are to
optimize our parallel code. ISO C++17 compilers will be unaware of how to
generate optimal code for core abstractions, such as :term:`queues` and
:term:`unified shared memory`.
Fortunately, there are many to choose from!

.. figure:: img/sycl_impls+backends.svg
   :align: center

   SYCL_ is an open standard for heterogeneous parallelism build on top of
   modern ISO C++. The `Khronos group <https://www.khronos.org/>`_ spearheads the
   standardization effort.  There are numerous compilers implementing the SYCL_
   standard and they use different **backends* to target CPUs, GPUs, and even
   specialized hardware such as field-programmable gate arrays (:term:`FPGA`)
   and `vector engines <https://en.wikipedia.org/wiki/Vector_processor>`_.  The
   figure shows the route, as of today, from SYCL_ source code to hardware
   through the various implementations and backends.
   The red, dashed lines show experimental backends, as of today.  The figure
   was adapted `from here <https://www.khronos.org/sycl/>`_ and `here
   <https://github.com/illuhad/hipSYCL/raw/develop/doc/img/sycl-targets.png>`_.


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

   - SYCL_ is an *open-source standard* to describe parallelism on heterogeneous
     platforms using the C++ programming language.
   - SYCL_ is *single-source*, *heterogeneous*, and *asynchronous*.
   - There are many implementations of the SYCL_ standard, targeting diverse hardware with different strategies:

     * hipSYCL_,
     * `Intel DPC++ <https://software.intel.com/content/www/us/en/develop/tools/oneapi/data-parallel-c-plus-plus.html#gs.cm2rrt>`_,
     * `CodePlay ComputeCPP <https://developer.codeplay.com/products/computecpp/ce/home/>`_.
     * `triSYCL <https://github.com/triSYCL/triSYCL>`_.
     * `neoSYCL <https://dl.acm.org/doi/abs/10.1145/3432261.3432268>`_.


.. rubric::

.. [#f1] Kokkos, Raja, and Alpaka are other frameworks with a similar approach to parallel programming.
