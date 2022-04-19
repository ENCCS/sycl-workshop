.. _sub-groups:

Using sub-groups in SYCL
========================

.. questions::

   - Is it always necessary to use work-group local memory and synchronization?

.. objectives::

   - Learn about sub-groups as an alternative to work-group local memory.

In episode :ref:`task-graphs-synchronization`, we saw that work-group local
memory is an effective means to achieve kernel-level communication between
work-items in an ND-range.
SYCL_ also offers *sub-groups* as a useful abstraction to achieve such
communication patterns.
Let's have a further look at the organization of a 3-dimensional :math:`8\times
8 \times 8` ND-range.

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

The number of work-items is dictated by the total size of the ND-range, here we
have 512 work-items, while we determine the subdivision in work-groups with our
definition of the local range, here :math:`4\times 4\times 4`.
Based on the SYCL_ implementation at hand, the further division into *sub-groups* might also be available.
The following three facts should be kept in mind when considering workign with sub-groups:

- They are always 1-dimensional and laid out along the contiguous dimension of
  their parent work-groups.
- Their size is implementation-dependent: it cannot be set in the ``nd_range``
  constructor.
- You can obtain the sizes of sub-groups with the ``info::device::sub_group_sizes`` query to ``get_info``:

  .. code:: c++

     std::cout << " Subgroup sizes: ";
     for (const auto& x :
          Q.get_device().get_info<info::device::sub_group_sizes>()) {
       std::cout << x << " ";
     }
     std::cout << std::endl;

Why would it be advantageous to use sub-groups? Sub-groups are groupings of
work-items within a given work-group with additional scheduling guarantees. The
most obvious guarantee is that all work-items in the sub-group **execute
together on the same hardware**. There is thus limited or no need to explicitly
synchronize work-items within a sub-group using barriers, as we did in the tiled
MatMul exercise.
Furthermore, since the details of sub-groups are left up to the implementors of
the SYCL_ standard, they might enjoy further execution guarantees and
performance optimizations over the use of work-group local memory. Communication
and synchronization might be more efficient for sub-groups than for work-group
local memory.

Within a ND-range ``parallel_for``, we access the sub-group to which the current ``nd_item``
belongs by calling the ``get_sub_group`` function.

.. signature:: ``get_sub_group``

   .. code:: c++

      sub_group get_sub_group() const;

For example:

.. code:: c++

   cgh.parallel_for(nd_range { global, local }, [=](nd_item<2> it) {
     // get subgroup
     auto sg = it.get_sub_group();
   });

When using work-group local memory, we explicitly set its values and synchronize
the memory view across the work-group using a barrier:

.. code:: c++

   tileA[i] = accA[m][l + i];
   it.barrier();

   for (auto k = 0; k < tile_sz; ++k) {
     sum += tileA[k] * accB[l + k][n];
   }


When using sub-groups, we use **collective functions** instead. A work-item in
the sub-group loads data and the **broadcasts** it to communicate it to all
elements in the sub-group:

.. code:: c++

   auto tileA = accA[m][l + i];

   for (auto k = 0; k < tile_sz; ++k) {
     sum += group_broadcast(sg, tileA, k) * accB[l + k][n];
   }

The ``group_broadcast`` function takes the value of the work-item with local id
``k`` in ``tileA`` and communicates it to all work-items in the ``sg``
sub-group:

.. signature:: ``group_broadcast``

   .. code:: c++

      template <typename Group, typename T>
      T group_broadcast(Group g, T x, Group::id_type local_id);

Since **all work-items** in the sub-group will perform the broadcast, the
``local_id`` argument **must be** the same for all work-items.

.. exercise:: Sub-group tiled MatMul

   We will modify the tiled MatMul exercise in
   :ref:`task-graphs-synchronization` to use sub-groups instead of local memory.
   Using ``group_broadcast`` removes all explicit work-item barriers in the
   code.

   **Don't do this at home, use optimized BLAS!**

   You can find a scaffold for the code in the
   ``content/code/day-3/00_sub_group-matmul/sub_group-matmul.cpp`` file,
   alongside the CMake script to build the executable. You will have to complete
   the source code to compile and run correctly: follow the hints in the source
   file.  A working solution is in the ``solution`` subfolder.

   #. As usual, we first create a queue and map it to the GPU, either explicitly:

      .. code:: c++

         queue Q{gpu_selector{}};

      or implicitly, by compiling with the appropriate ``HIPSYCL_TARGETS`` value.

   #. We first declare the operands as ``std::vector<double>`` the
      right-hand side operands are filled with random numbers:

      .. code:: c++

         // initialize input and output memory on the host
         constexpr size_t sz = 256;

         // we could make the matrices non-square!
         constexpr size_t M = sz;
         constexpr size_t N = sz;
         constexpr size_t K = sz;


         std::vector<double> a(M * K), b(K * N);

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

      We can choose not to allocate space on the host for the result matrix up
      front, but rather rely on a ``host_accessor`` to do the job once the
      computation completes, see :ref:`task-graphs-synchronization`.

   #. We define buffers for all the operands in our matrix multiplication:

      .. code:: c++

         buffer<double, 2> bufA(a.data(), range{M, N}), bufB(b.data(), range{N, K});
         buffer<double, 2> bufC(range{M, K});

      Once again, note that the buffer for the result matrix is not associated
      to any allocated memory on the host.
      
   #. We submit work to the queue through a command group handler:

      .. code:: c++

         Q.submit([&](handler& cgh) {
           /* work for the queue */
         });

   #. We declare accessors to the buffers. For example, for the matrix :math:`\mathbf{A}`:

      .. code:: c++

         accessor accA{ a_buf, cgh, read_only };

      It is convenient to use access targets and properties, such as
      ``read_only``, since they can help the scheduler optimize memory accesses.
      We know that both :math:`\mathbf{A}` and :math:`\mathbf{B}` are read-only,
      whereas :math:`\mathbf{C}` is write-only and any data it holds before
      entering the kernel can be discarded.

   #. We also need to define the global and local 2-dimensional iteration
      ranges:

      .. code:: c++

         constexpr auto tile_sz = 16;

         range global{M, K};
         range local{1, tile_sz};

      The former will span the whole result matrix, while the latter spans
      *tiles* of the rows of :math:`\mathbf{A}`.  The tile size should be a
      value lesser than or equal to the sub-group size.

   #. Within the handler, we launch a ``parallel_for``. The parallel
      region iterates over the 2-dimensional ranges of global and local
      indices.

      .. code:: c++

         cgh.parallel_for(
           nd_range{ /* global range */, /* local range */ },
           [=](nd_item<2> it){
             /* kernel body */
           }
         );

      In the kernel body:

      - For each ``nd_item``, we get its subgroup, the global ids (row and
        column indices), and the local id in the first dimension (the index
        within a tile):

        .. code:: c++

           // get subgroup
           auto sg = it.get_sub_group();

           // indices in the global index space
           // these are used to address
           //  - the result (held in global memory)
           //  - the row of the left operand
           //  - the column of the right operand
           auto m = it.get_global_id()[0];
           auto n = it.get_global_id()[1];

           // index in the local index space
           auto i = it.get_local_id()[1];

      - We load the :math:`\mathbf{A}` matrix tile by tile:

        .. code:: c++

           // accumulate result
           auto sum = 0.0;
           // loop over inner index (common to operands) with stride equal to the
           // tile size
           for (auto l = 0; l < K; l += tile_sz) {
             // load a tile of matrix A
             auto tileA = accA[m][l + i];

      - We loop over elements in the tile, *broadcast* them to the whole
        sub-group and accumulate the value of matrix multiplication:

        .. code:: c++

            // loop over tile elements
            for (auto k = 0; k < tile_sz; ++k) {
              // broadcast tile element to the subgroup and load matrix B from
              // global memory
              sum += group_broadcast(sg, tileA, k) * accB[l + k][n];
             }
           }

      - Outside the double loop, we assign the compute value to the appropriate
        element in the result matrix :math:`\mathbf{C}`:

        .. code:: c++

           // finally, write to the result matrix
           accC[m][n] = sum;

   #. Copy back the results to the host using a ``host_accessor``:

      .. code:: c++

         host_accessor C{bufC};

      and check that your results are correct.


.. keypoints::

   - Sub-groups are an implementation-defined grouping of work-items within a
     work-group with additional scheduling guarantees.
   - The use of sub-groups might improve your code by removing, partially or
     completely, the use of explicit synchronization barriers.
   - Group-collective functions, such as ``group_broadcast``, might offer a
     better alternative than using explicit work-group local memory and
     synchronization, especially in a high-quality SYCL_ implementation.


.. rubric:: Footnotes

.. [*] Consuld the SYCL 2020 standard for a complete list of `group-collective
       functions
       <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:group-functions>`_
       and `group-collective algorithms
       <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:algorithms>`_
