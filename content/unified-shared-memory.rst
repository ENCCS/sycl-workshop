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

.. table:: **Kinds of USM memory allocations and their properties**. SYCL offers an
           allocator- and ``malloc``-style APIs. In the latter, typed (returning
           ``T**``) and untyped (returning ``void*``) functions are available,
           either with the kind explicitly in the name, *e.g.* ``malloc_host``,
           or accepting it as an extra type parameter.

   .. csv-table::
      :header: "Kind", "Host-accessible", "Device-accessible" , "Memory space" , "Automatic migration"
      :widths: auto
      :delim: ;

      ``device``         ;      No          ;        Yes         ; Device           ; No
      ``host``           ;      Yes         ;        Yes         ; Host             ; No
      ``shared``         ;      Yes         ;        Yes         ; Shared           ; Yes


USM data movement
-----------------

USM supports both *explicit* and *implicit* data movement strategies.

.. todo::

   See page 66 and following.

.. exercise:: AXPY with SYCL and :term:`USM`

   We will now write an AXPY implementation in SYCL_, using :term:`unified
   shared memory`. This will be a generic implementation: it will work with any
   arithmetic type, thanks to C++ templates.

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

   - Unified shared memory (USM) provides a pointer-based API for SYCL.
   - It is useful when porting existing code to SYCL, as it might require less
     pervasive changes to the codebase.
   - USM might bring better performance and more opportunities for fine-grained
     performance tuning.
