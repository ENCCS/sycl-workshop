.. _expressing-parallelism-nd-range:

Expressing parallelism with SYCL: nd-range data-parallel kernels
================================================================

.. questions::

   - How do we write parallel kernels in SYCL_?

.. objectives::

   - Learn the difference between *task* and *data* parallelism.
   - Learn about *work-items*, *work-groups*, and *ND-ranges*.


ND-range data-parallel kernels
------------------------------

The basic flavor of data-parallel kernel allows to map 1-, 2-, and 3-dimensional
problems to the available hardware. However, the semantics of the ``range``
class is quite limited: we have no way of expressing notions of locality within
these kernels.
This was quite evident in the matrix multiplication exercise: each work-item
addressed one element in the output matrix and would load a full (contiguous)
row of the left operand and a full (non-contiguous) column of the right operand.
Loading of operands would happen *multiple* times.

ND-ranges are represented with objects of type ``nd_range``, templated over the number of dimensions:

.. signature:: ``nd_range`` constructor

   .. code:: c++

      nd_range(range<dimensions> globalSize, range<dimensions> localSize);

these are constructed using *two* ``range`` objects, representing the
*global* and *local* execution ranges:

- The *global range* gives the total size of the ``nd_range``: a 1-, 2-, or
  3-dimensional collection of **work-items**. This is exactly like ``range``
  objects: at their coarsest level the two objects look exactly the same.
- The *local range* gives the size of each **work-group** comprising the
  ``nd_range``.
- The implementation can further subdivide each work-group into 1-dimensional
  **sub-groups**. Since this is an implementation-dependent feature, its size
  cannot be set in the ``nd_range`` constructor.

Note that:

1. The local sizes in each dimension have to divide exactly the corresponding
   global sizes.
2. The contiguous dimensions of the ND-range and its work-groups coincide.
3. Sub-groups, if available, are along the contiguous dimension of their
   work-groups.

.. figure:: img/nd_range.svg
   :align: center
   :scale: 50%

   Depiction of a 3-dimensional ND-range and its further subvisions. The global
   execution range is :math:`8\times 8 \times 8`, thus containing 512
   **work-items**. The global range is further subdivided into 8 **work-groups**
   each comprised of :math:`4 \times 4 \times 4` work-items. At an even finer
   level, each work-group has **sub-groups** of 4 work-items.
   The availability of sub-groups is implementation-dependent.
   Note that the contiguous dimension (dimension 2 in this example) of ND-range
   and work-group coincide. Furthermore, sub-groups are laid out along the
   contiguous dimension of their work-groups.
   Figure adapted from :cite:`Reinders2021-yx`.

Work-groups and work-items in ND-ranges
---------------------------------------

As for basic ranges, the work-items in an ND-range represent instances of a kernel function:

- they cannot communicate, nor sychronize with each other. [*]_
- they are scheduled for execution in any order.

Work-groups are the novelty of ND-ranges and the fact that work-items are
grouped together gives us more programming tools:

- Each work-group has *work-group local memory*. Each work-item in the group can
  access this memory. How the memory is made available is up to the hardware and
  the SYCL implementation.
- We can use group-level *barriers* and *fences* to synchronize
  work-items within a group.
- We can use group-level collectives, for communication, *e.g.* broadcasting, or
  computation, *e.g.* scans.

Work-items in a work-group are scheduled **concurrently** to a single compute
unit, however:

- There can be many more work-groups in an ND-range than compute units in the
  hardware. It's fine to oversubscribe the hardware and if we avoid to make our
  code *too* device-specific, we have a better chance at achieving portability.
- Work-items are **not** guaranteed to make *independent* progress. Interleaving
  execution of a work-item with barriers and fences, effectively running in a
  sequential fashion, *is* a valid execution model and the runtime may decide to
  do just so.


In ND-range data-parallel kernels, the kernel function passed as second argument
to the ``parallel_for`` invocation accepts objects of type ``nd_item``, which
generalize the ``item`` type.
The ``nd_item`` gives access to its ids in the global and local ranges with the
``get_global_id`` and ``get_local_id`` methods, respectively. It is often
clearer, however, to first obtain a handle to the work-group (sub-group) with
the ``get_group`` (``get_sub_group``) method and then interrogate ids and ranges
from the returned ``group`` (``sub_group``) objects.


.. exercise:: Less naïve MatMul

   Using the ND-range flavor of data-parallelism should let us optimize memory
   accesses a bit more.  In this exercise, we will rewrite the matrix
   multiplication kernel to use ``nd_range`` s.

   Each work-item will compute an element in the result matrix
   :math:`\mathbf{C}` by accessing a full row of :math:`\mathbf{A}` and a full
   column of :math:`\mathbf{B}`.  However, at variance with the previous
   implementation, the work-item is in a work-group, and thus the data loaded
   for the operands can be reused by all work-items, improving locality of
   accesses.

   .. figure:: img/less_naive_matmul.svg
      :align: center

      Schematics of a *less* naïve implementation of matrix multiplication:
      :math:`C_{ij} = \sum_{k}A_{ik}B_{kj}`. The computation is split into
      work-groups to optimize the locality of memory accesses. Each work-item
      (green) will compute an element in the result matrix
      :math:`\mathbf{C}` by accessing a full row of :math:`\mathbf{A}` and a
      full column of :math:`\mathbf{B}`.  However, since the work-item is in a
      work-group (orange), the data loaded for the operands can be reused by all
      work-items.
      Figure adapted from :cite:`Reinders2021-yx`.


   **Don't do this at home, use optimized BLAS!**

   .. tabs::

      .. tab:: Using buffers and accessors

         You can find a scaffold for the code in the
         ``content/code/day-2/02_nd_range-matmul/nd_range-matmul.cpp`` file,
         alongside the CMake script to build the executable. You will have to complete
         the source code to compile and run correctly: follow the hints in the source
         file.  A working solution is in the ``solution`` subfolder.

         #. We first create a queue and map it to the GPU, either explicitly:

            .. code:: c++

               queue Q{gpu_selector{}};

            or implicitly, by compiling with the appropriate ``HIPSYCL_TARGETS`` value.

         #. We declare the operands as ``std::vector<double>`` the
            right-hand side operands are filled with random numbers:

            .. code:: c++

               constexpr size_t N = 256;
               std::vector<double> a(N * N), b(N * N), c(N * N);

               // fill a and b with random numbers in the unit interval
               std::random_device rd;
               std::mt19937 mt(rd());
               std::uniform_real_distribution<double> dist(0.0, 1.0);

               std::generate(a.begin(), a.end(), [&dist, &mt]() {
                 return dist(mt);
               });
               std::generate(b.begin(), b.end(), [&dist, &mt]() {
                 return dist(mt);
               });

            while the result matrix is zeroed out:

            .. code:: c++

               std::vector<double> c(N * N);

               // zero-out c
               std::fill(c.begin(), c.end(), 0.0);

         #. We define buffers to the operands in our matrix multiplication. For
            example, for the matrix :math:`\mathbf{A}`:

            .. code:: c++

               buffer<double, 2> a_buf(a.data(), range<2>(N, N));

            Since we will be using the ND-range version, we will also need a
            local, 2-dimensional iteration range, with size :math:`B\times B`:

            .. code:: c++

               constexpr size_t B = 4;

         #. We submit work to the queue through a command group handler:

            .. code:: c++

               Q.submit([&](handler& cgh) {
                 /* work for the queue */
               });

         #. We declare accessors to the buffers. For example, for the matrix :math:`\mathbf{A}`:

            .. code:: c++

               accessor a{ a_buf, cgh };

            We also need the global and local 2-dimensional iteration ranges:

            .. code:: c++

               range global{N, N};
               range local{B, B};

         #. Within the handler, we launch a ``parallel_for``. The parallel
            region iterates over the 2-dimensional ranges of global and local
            indices, with an inner loop to span the common dimension of the
            :math:`\mathbf{A}` and :math:`\mathbf{B}` operand matrices:

            .. code:: c++

               cgh.parallel_for(
                 nd_range{ /* global range */, /* local range */ },
                 [=](nd_item<2> it){
                   auto j = it.get_global_id(0);
                   auto i = it.get_global_id(1);
                   for (auto k = 0; k < N; ++k) {
                     c[j][i] += ...;
                   }
                 }
               );

         #. Check that your results are correct.

      .. tab:: Using USM

         You can find a scaffold for the code in the
         ``content/code/day-2/03_usm-nd_range-matmul/usm-nd_range-matmul.cpp`` file,
         alongside the CMake script to build the executable. You will have to complete
         the source code to compile and run correctly: follow the hints in the source
         file.  A working solution is in the ``solution`` subfolder.

         #. We first create a queue and map it to the GPU, either explicitly:

            .. code:: c++

               queue Q{gpu_selector{}};

            or implicitly, by compiling with the appropriate ``HIPSYCL_TARGETS`` value.

         #. We allocate the operands as USM buffers and fill them with random
            numbers. We can do this with untyped or typed ``malloc``-style or
            ``usm_allocator`` APIs. Should operands be host, device, or shared
            allocations?
         #. We allocate the result as USM buffer and zero it out.  We can do
            this with untyped or typed ``malloc``-style or ``usm_allocator``
            APIs. Should this be host, device, or shared allocation?
         #. We submit work to the queue. Note that we need to linearize indices
            for row-major access to our buffers:

            .. code:: c++

               auto irow = ...;
               auto jcol = ...;
               auto row_major_id = irow * N + jcol;

         #. Check that your results are correct.


.. keypoints::

   - ND-range kernels should be used for more sophisticated control over
     performance aspects.


.. rubric:: Footnotes

.. [*] Atomic operations are a way to synchronize work-items, but we will not go
       into that in this workshop.
