.. _what-is-sycl:

What is SYCL?
=============

.. questions::

   - What is SYCL_ and why should I care?

.. objectives::

   - Learn how to write a simple C++ SYCL_ code.
   - Learn how to compile with hipSYCL_.


The high-performance computing landscape is increasingly dominated by machines
whose high FLOP count is delivered by *heterogeneous hardware*: large-core
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

      .. literalinclude:: code/snippets/saxpy.cu
         :language: cuda

      This code is an example of *prescriptive* parallelism.

   .. tab:: HIP

      .. literalinclude:: code/snippets/saxpy.hip.cpp
         :language: c++

      This code is an example of *prescriptive* parallelism.

   .. tab:: OpenMP

      .. literalinclude:: code/snippets/saxpy.omp.cpp
         :language: c++

      This code is an example of *descriptive* parallelism.

   .. tab:: OpenACC

      .. literalinclude:: code/snippets/saxpy.acc.c
         :language: c

      This code is an example of *descriptive* parallelism.


There are few things to notice in this example:

- We work with *prescriptive* parallelism with CUDA_ and HIP_: we explicitly
  divide up the work between threads inside the kernel function.
- In the OpenMP_ and OpenACC_ models, we describe data movement and worksharing,
  but rely on the compiler to generate correct code. Hence the qualification of
  *descriptive* parallelism.
- When using low-level APIs, a certain degree of code duplication between host
  and device code **is inevitable**. Parallelization strategies between host
  and device will be different.  On top of this, the expressiveness of
  low-level language extensions is limited, which might increase the
  maintenance burden.
- Low-level APIs might only be available with proprietary, vendor-specific
  compilers. This limits both *functional* and *performance* portability.
- ``pragma``-based schemes are standardized, but compiler support is not always
  optimal.

When programming for heterogeneous platforms, there is no silver bullet: we have
to carefully evaluate the tradeoffs between performance, productivity, and
portability.
SYCL_ is an attempt [#f1]_ at improving this state of affairs. It is a
*standards-based* and *vendor-agnostic* domain-specific embedded language for
parallel programming, for heterogeneous and homogeneous architectures.
The SYCL_ standard is developed by the `Khronos group <https://www.khronos.org/>`_:

* It is built as a **header-only** library for ISO C++17. SYCL_ code can be
  compiled with a standards-compliant compiler and the necessary headers: it
  does **not** require special compiler extensions.
* It is a **single-source**-style framework. Host and device code are in the
  same *translation unit*.
* It is **asynchronous**. The programmer describes computations, memory
  allocations, and data migrations; the runtime generates a task graph and works
  its way through it when executing the program.

The SYCL compiler
-----------------

Despite the fact that SYCL is a header-only library for ISO C++17, we will still
need a SYCL-aware compiler.
ISO C++17 compilers will be unaware of how to generate optimal code for core
abstractions, such as :term:`queues` and :term:`unified shared memory`.
A SYCL compiler will be able to optimize our parallel code and will also be
aware of which low-level framework to use to target specific architectures.
Fortunately, there are many SYCL implementations to choose from!

.. figure:: img/sycl_impls+backends.svg
   :align: center

   SYCL_ is an open standard for heterogeneous parallelism build on top of modern ISO C++.
   The `Khronos group`_ spearheads the standardization effort.
   There are numerous compilers implementing the SYCL_ standard and they use
   different **backends** to target CPUs, GPUs, and even specialized hardware
   such as field-programmable gate arrays (:term:`FPGA`) and :term:`vector engines`.
   The figure shows the route, as of today, from SYCL_ source code to hardware
   through the various implementations and backends.  The red, dashed lines show
   experimental backends, as of today.  The figure was adapted `from here`_ and
   here_.

.. _`Khronos group`: https://www.khronos.org/
.. _`from here`: https://www.khronos.org/sycl/
.. _`here`: https://github.com/illuhad/hipSYCL/raw/develop/doc/img/sycl-targets.png


Hello, SYCL!
------------

Let's dig in with a "Hello, world" example.

.. typealong:: "Hello, world" with SYCL

   You can find the file with the complete source code in the
   ``content/code/day-1/00_hello`` folder. Worry not about the details in the
   code, we will dig into what is happening here at great length during the rest
   of the lesson.

   .. literalinclude:: code/day-1/00_hello/hello.cpp
      :language: c++
      :lines: 7-
      :emphasize-lines: 25-27

   #. Log in onto `Vega <https://doc.vega.izum.si/login/>`_ and clone the
      repository for this workshop. Navigate to the correct folder. This
      contains a source file, ``hello.cpp``, and the CMake script to build it.

   #. Load the necessary modules:

      .. code:: console

         $ module load CMake hipSYCL

   #. Configure and compile the code:

      .. code:: console

         $ cmake -S. -Bbuild -DHIPSYCL_TARGETS="omp"
         $ cmake --build build -- VERBOSE=1

   #. Run the code! What result do you get?

      .. code:: console

         ./build/hello

   #. We can configure again to target the GPU:

      .. code:: console

         $ cmake -S. -Bbuild -DHIPSYCL_TARGETS="cuda:sm_80"
         $ cmake --build build -- VERBOSE=1
         $ ./build/hello

      What output do you see? We will talk more about *device selection* in :ref:`device-discovery`.

This source code introduces a number of fundamental concepts in SYCL_:

#. SYCL is a template library and its classes and functions are behind the
   ``sycl::`` namespace.  The SYCL runtime is provided by an optimizing
   compiler, in our case hipSYCL_:

   .. code:: c++

      #include <sycl/sycl.hpp>

      using namespace sycl;

#. Host and device code are in the same translation unit.
#. Thanks to **unified shared memory** we can use a pointer-based approach to
   memory management that transparently works *across* host and devices:

   .. code:: c++

      char *result = malloc_shared<char>(sz, Q);
      std::memcpy(result, secret.data(), sz);

   We still need to manage host-to-device and device-to-host memory migrations.
   SYCL_ offers methods to avoid this, which we will cover in
   :ref:`buffers-accessors` and :ref:`unified-shared-memory`.

#. A **queue** is the mechanism by which we orchestrate work on our devices.
   For example, getting the device on which our **actions** will run:

   .. code:: c++

      queue Q;
      Q.get_device().get_info<info::device::name>();

   We will explore ``get_info`` and mechanisms for device selection in the
   :ref:`device-discovery` section.

#. An **action** is submitted to a queue and it runs on a device. In this
   example, our action is a ``parallel_for`` on a 1-dimensional **range** of
   work items

   .. code:: c++

      Q.parallel_for(
         range<1>{sz},  /* range of work items */
         ...
      );

#. Within actions, we execute **kernels**:

   .. code:: c++

      [=](id<1> tid) {
        result[tid[0]] -= 1;
      }

   the ``result`` array is indexed using an ``id`` object: a mapping between a
   ``range`` of work items and available workers.
   Kernels are either `lambda functions <https://en.cppreference.com/w/cpp/language/lambda>`_ or
   `function objects <https://en.cppreference.com/w/cpp/utility/functional>`_.

#. Actions are executed **asynchronously**. The host enqueues work and moves on
   with its tasks. If results are neeeded from an action, then we need to wait
   for it to complete:

   .. code:: c++

      Q.parallel_for(
         range<1>{sz},        /* range of work items */
         [=](id<1> tid) {     /* kernel code */
           result[tid[0]] -= 1;
         }
      ).wait();

We have introduced two functions to manage :term:`USM`:
``sycl::malloc_shared`` and ``sycl::free``, for memory allocation and deallocation:

.. signature:: ``sycl::malloc_shared``

   .. code:: c++

      template <typename T>
      T* sycl::malloc_shared(size_t count,
                             const queue& syclQueue,
                             const property_list &propList = {})

.. parameters::

   ``T``
       Type of the allocation.
   ``count``
       How many elements to allocate.
   ``syclQueue``
       Queue on which to perform the allocation.
   ``propList``
       Properties of the allocation.

.. signature:: ``sycl::free``

   .. code:: c++

      void sycl::free(void* ptr, sycl::queue& syclQueue)

.. parameters::

   ``ptr``
         Memory to deallocate. It **must** have been allocated using any of the
         :term:`USM` functions.
   ``syclQueue``
         Queue on which to perform the deallocation.


We have handled memory migration using ``std::memcpy``, which is part of the `C++ standard <https://en.cppreference.com/w/cpp/string/byte/memcpy>`_.


.. challenge:: AXPY with SYCL

   We will now write an AXPY implementation in SYCL_. This will be a generic
   implementation: it will work with any arithmetic type, thanks to C++
   templates.
   We will use :term:`unified shared memory` and revisit this implementation in
   a later exercises to use *buffers* and *accessors* instead.
   We will dive deeper into these concepts in episodes
   :ref:`unified-shared-memory` and :ref:`buffers-accessors`.

   **Don't do this at home, use optimized BLAS!**

   You can find a scaffold for the code in the
   ``content/code/day-1/01_axpy-usm/axpy.cpp`` file, alongside the CMake script
   to build the executable. You will have to complete the source code to compile
   and run correctly: follow the hints in the source file.  The solution is in
   the ``solution`` subfolder.

   #. Load the necessary modules:

      .. code:: console

         $ module load CMake hipSYCL

   #. Configure, compile, and run the code:

      .. code:: console

         $ cmake -S. -Bbuild -DHIPSYCL_TARGETS="omp"
         $ cmake --build build -- VERBOSE=1
         $ ./build/axpy

      You can use ``cuda:sm_80`` to compile for the GPU.




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


.. rubric:: Footnotes

.. [#f1] Kokkos, Raja, and Alpaka are other frameworks with a similar approach to parallel programming.
