.. _unified-shared-memory:


Data management with unified shared memory
==========================================

.. questions::

   - What facilities does SYCL offer to manage memory spaces in an heterogeneous environment?

.. objectives::

   - Learn about the unified shared memory (USM) API.


In the previous episode we learnt that SYCL_ offers three abstractions for
memory management: the buffer and accessor API and unified shared memory (USM)
are the most relevant for our purposes. We will discuss the latter in this episode.

USM is probably the biggest new feature adopted in the SYCL 2020 standard. Why?
The value of any pointer returned by a USM allocation on the host is
*guaranteed* to be a valid pointer value also on the device.
We have seen that the buffer-accessor API is powerful and also quite intuitive
in a modern C++ setting. However, most programmers are quite familiar with
pointer-based memory management, especially if they have been working with
low-level CUDA/HIP languages. Furthermore, it is difficult to adopt SYCL in an
existing codebase when it requires radical changes in fundamental
infrastructure. USM offers a path forward.

.. warning::

   USM will only be available in SYCL for devices that support a *unified
   virtual address space*. If you want/need to use USM, be sure to write an
   appropriate selector for queue!


Let's now analyze the allocation and data movement aspects of USM.

USM memory allocation
---------------------

There are three types of USM allocations available in the SYCL standard. If you
have worked previously with CUDA/HIP, this "menu" of allocations should look
fairly familiar to you:

- Device allocations will return a pointer to memory physically located on the
  device.
- Host allocations will return a pointer to memory physically located on the
  host. These are accessible both on the host *and* the device. In the latter
  case, however, memory will not migrate to the device automatically, but rather
  be accessed remotely. This is a crucial aspect to keep in mind for
  performance!
- Shared allocations will return a pointer to the unified virtual address space.
  Such allocations can be accessed from both host and device, and the memory can
  migrate freely, without programmer intervention, between host and device. This
  comes at the cost of increasing latency.

The following table summarizes the available USM allocations and their properties.

.. table:: **Kinds of USM memory allocations and their properties**. SYCL offers an
           allocator- and ``malloc``-style APIs. In the latter, typed (returning
           ``T*``) and untyped (returning ``void*``) functions are available,
           either with the kind explicitly in the name, *e.g.* ``malloc_host``,
           or accepting it as an extra type parameter.

   .. csv-table::
      :header: "Kind", "Host-accessible", "Device-accessible" , "Memory space" , "Automatic migration"
      :widths: auto
      :delim: ;

      ``device``         ;      No          ;        Yes         ; Device           ; No
      ``host``           ;      Yes         ;        Yes         ; Host             ; No
      ``shared``         ;      Yes         ;        Yes         ; Shared           ; Yes

To perform USM allocations, we need to inform the runtime about which device
we'd like to request memory from. The simplest way is to pass a ``queue`` object
to the allocation functions. The standard provides three APIs for allocating
USM:

- C-like (untyped)

  .. signature:: C-like ``malloc``

     .. code:: c++

        void* malloc_device(size_t numBytes,
                            const queue& syclQueue,
                            const property_list& prop_list = {});

        void* malloc_host(size_t numBytes,
                          const queue& syclQueue,
                          const property_list& prop_list = {});

        void* malloc_shared(size_t numBytes,
                            const queue& syclQueue,
                            const property_list& prop_list = {});

- C++-like (typed)

  .. signature:: C++-like ``malloc``

     .. code:: c++

        template <typename T>
        T* malloc_device(size_t count,
                         const queue& syclQueue,
                         const property_list& prop_list = {});

        template <typename T>
        T* malloc_host(size_t count,
                       const queue& syclQueue,
                       const property_list& prop_list = {});

        template <typename T>
        T* malloc_shared(size_t count,
                         const queue& syclQueue,
                         const property_list& prop_list = {});

- C++ allocator object ``usm_allocator``.

As usual, you need to free any memory you claim dynamically from the runtime.
The ``free`` function also needs information about the location of the memory,
which can be conveniently conveyed by a ``queue`` object:

.. signature:: ``free``

   .. code:: c++

      void free(void* ptr,
                queue& syclQueue);

USM data management
-------------------

We have claimed memory from the system, now what to do with it? Usually, we
first initialize with some more-or-less useful values and then use it in our
data-parallel kernels.

Initialization of the allocated memory to a byte or to an arbitrary
pattern can be achieved using the  ``memset`` and ``fill`` functions,
respectively, provided by the SYCL standard.

.. demo:: Using ``fill``

   .. code:: c++

      Q queue;
      auto x = malloc_device<double>(256, Q);
      fill(x, 42.0, 256);

For more complex initialization, a data-parallel loop is the way to go and it
requires us to learn about USM and data movement.

Movement
~~~~~~~~

Data movement is a big part of achieving performanance in a heterogeneous
programming environment: data needs to be available at the right time *and* at
the right place for parallel kernels to perform optimally.
This is probably old news already, if you come from a CUDA/HIP approach.

USM supports both *explicit* and *implicit* data movement strategies.

Explicit
  We have to call ``memcpy`` (untyped C-like API) and ``copy`` (typed C++-like
  API) explicitly whenever data needs to migrate between different backends.
  These methods are available both on the ``queue`` and ``handler`` classes.
  **Remember** methods of the ``queue`` and ``handler`` class are
  **asynchronous**! Copies are not an exception!
  Explicit data movement is only strictly necessary for host-to-device and
  device-to-host data migrations. Indeed, device allocations cannot be directly
  accessed from the host.

  .. demo::  Explicit data migration

     This looks quite like CUDA/HIP!
     But why would want to use this? Isn't the whole point of SYCL to **not
     think** about data movements?  While ease of programming is definitely
     important, we also want a framework that empowers us to take full control
     whenever we deem it necessary.

     .. tabs::

        .. tab:: SYCL

           .. code:: c++

              constexpr auto N = 256;
              queue Q;

              std::vector<double> x_h(N);
              std::iota(x_h.begin(), x_h.end(), 0.0);

              auto x_d = malloc_device<double>(N, Q);

              // in a handler
              Q.submit([&](handler& cgh){
                // untyped API
                cgh.memcpy(x_d, x_h.data(), N*sizeof(double));
                // or typed API
                //cgh.copy(x_d, x_h.data(), N);
              });

              // or on the queue directly
              // with typed API
              //Q.copy(x_d, x_h.data(), N);
              //or untype API
              //Q.memcpy(x_d, x_h.data(), N*sizeof(double));

              // copies are ASYNCHRONOUS!!
              Q.wait();

        .. tab:: CUDA

           .. code:: c++

              constexpr auto N = 256;

              std::vector<double> x_h(N);
              std::iota(x_h.begin(), x_h.end(), 0.0);

              double* x_d;
              cudaMalloc((void**)&x_d, N*sizeof(double));

              cudaMemcpy(x_d, x_h.data(), N*sizeof(double), cudaMemcpyHostToDevice);

Implicit
  This movement strategy requires no programmer intervention and is relevant for
  host and shared allocations. When the former are accessed on a device, the
  runtime will transfer the memory through the appropriate hardware interface.
  Host memory allocations do not migrate to the device, so they incur latency
  and repeated accesses are discouraged.
  Shared memory is essentially defined by its ability to migrate between host
  and device. This happens simply by accessing the same memory location from
  different locations.

  .. demo:: Host and shared allocations benefit from implicit data movement

     In this sample code, we access both the ``x_h`` and ``x_s`` arrays within kernel code.
     The former will be transferred from host memory over the appropriate
     interface, *e.g.* PCIe. The latter will be migrated to the device memory.

     .. code:: c++

        constexpr auto N = 256;
        queue Q;

        auto x_h = malloc_host<double>(N, Q);
        for (auto i = 0; i < N; ++i) {
          x_h[i] = static_cast<double>(i);
        }

        auto x_s = malloc_shared<double>(N, Q);

        // in a handler
        Q.submit([&](handler& cgh){
          cgh.parallel_for(range{N}, [=](id<1> tid){
            // get index out of id object
            auto i = tid[0];
            x_s[i] = x_h[i] + 1.0;
          }
        });

        // or on the queue directly
        //Q.parallel_for(range{N}, [=](id<1> tid){
        //  // get index out of id object
        //  auto i = tid[0];
        //  x_s[i] = x_h[i] + 1.0;
        //}

        Q.wait();



.. exercise:: AXPY with SYCL and USM

   .. tabs::

      .. tab:: Using the ``malloc`` API and "raw" arrays

      .. tab:: Using the ``usm_allocator`` API and ``std::vector``

   We will now write an AXPY implementation in SYCL, using unified shared
   memory. This will be a generic implementation: it will work with any
   arithmetic type, thanks to C++ templates.

   **Don't do this at home, use optimized BLAS!**

   You can find a scaffold for the code in the
   ``content/code/day-1/05_axpy-usm/axpy.cpp`` file, alongside the CMake script
   to build the executable. You will have to complete the source code to compile
   and run correctly: follow the hints in the source file.  The solution is in
   the ``solution`` subfolder.

   The code fills two ``std::vector`` objects and passes them to the ``axpy``
   function, which accepts a ``queue`` object as first parameter.
   You have to complete this function:

   #. Define buffers to view into the input and output vectors.
   #. Schedule work on the ``queue`` using a command group.
   #. Define accessors to the input and output vectors, with proper access mode
      and target.
   #. Write the AXPY kernel as a lambda function.
   #. Return the computed value.

   A working solution is in the ``solution`` subfolder.


.. keypoints::

   - Unified shared memory (USM) provides a pointer-based API for SYCL. It looks
     and feels familiar if coming from CUDA/HIP.
   - It is useful when porting existing code to SYCL, as it might require less
     pervasive changes to the codebase.
   - The SYCL standard offers APIs for host, device, and shared allocations.
   - USM supports both explicit and implicit data movement. The former is only
     relevant for device allocations.
   - Implicit data movement simplifies programmers' life considerably, but we
     might incur hard-to-control performance penalties.
