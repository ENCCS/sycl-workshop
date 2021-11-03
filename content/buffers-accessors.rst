.. _buffers-accessors:


Data management with buffers and accessors
==========================================

.. questions::

   - What facilities does SYCL offer to manage memory spaces in an heterogeneous environment?

.. objectives::

   - Learn about the buffer and accessor API.
   - Learn how the SYCL runtime manages memory.


Heterogeneous computing architectures offer a lot of computational power, but
harnessing it can be highly nontrivial. The existence of **multiple** memory
*regions* and *hierarchies* is one of the reasons for this.

.. figure:: img/CPUAndGPU.png
    :align: center

    A comparison of CPU and GPU architectures.  CPU (left) has complex core
    structure and pack several cores on a single chip.  GPU (right) cores are
    very simple in comparison, they also share data and control between each
    other.  This allows to pack more cores on a single chip, thus achieving very
    high compute density. [*]_
    Global memory, GPU memory, CPU, and GPU caches have to be managed, more or
    less explicitly, by the programmer to achieve optimal performance.

Any meaningful computation requires data in input and will produce some data as
output.  There needs to be transfer of data to and from the compute units and
migration of data has an impact on performance.
SYCL_ gives us the freedom to decide whether to perform data movements
**explicitly** or **implicitly**. The latter strategy delegates the problem to
the runtime: it reduces the opportunity for data-movement related bugs, but
leaves no room for hand-optimization.
With the former, we retain full control, but we will have to write tedious and
error-prone code for data transfer.

There are three memory-management abstractions in the SYCL_ standard:

#. **unified shared memory**. This is a pointer-based approach, familiar to
   C/C++ programmers and similar to CUDA/HIP low-level languages for
   accelerators. USM pointers on the host are valid pointers also on the device.
   This is at variance with "classic" pointers in CUDA/HIP.  USM needs device
   support for a *unified virtual address space*.
#. **the buffer and accessor API**. A *buffer* is a handle to a 1-, 2-, or
   3-dimensional memory location. It specifies where the memory *location* and
   where it can be *accessed*: host, device or both. As such, the buffer does
   not own the memory: it's only a *constrained view* into it. We don't work on
   buffer directly, but rather use *accessors* into them.  This is analogous to
   a :term:`RAII`-like approach, similar to what the :term:`STL` does in C++.
#. **images**. They offer a similar API to buffer types, with extra
   functionality tailored for image processing. We will not discuss images in
   this workshop.

In this episode, we will look at the buffer and accessor API, while the next
episode :ref:`unified-shared-memory` will discuss of USM. We will compare the
two methods in :ref:`buffer-accessor-vs-usm`.

   - Implicit memory management with buffers and accessors.
   - Dependencies in relation with the scheduling done by the SYCL runtime.


.. exercise:: AXPY with SYCL buffers and accessors

   We will now write an AXPY implementation in SYCL_, using the buffer and
   accessor API.  This will be a generic implementation: it will work with any
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

   - Buffers and accessors delegate memory management issues to the SYCL runtime.
   - SYCL lets you abstract away the intricacies of host-device data dependencies.
   - It can be hard to adapt an existing code to the buffer-accessor model.
   - There might be performance overhead when adopting the buffer-accessor model.


.. rubric:: Footnotes

.. [*] Reproduced, with permission, from the training material for the `ENCCS CUDA workshop <https://enccs.github.io/CUDA/1.01_GPUIntroduction/#graphics-processing-units>`_.
