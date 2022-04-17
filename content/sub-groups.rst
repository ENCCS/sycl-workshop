.. _sub-groups:

Using sub-groups in SYCL
========================

.. questions::

   .. todo::

      WRITE ME!

.. objectives::

   .. todo::

      WRITE ME!


What are sub-groups?
--------------------

.. todo::

   WRITE ME!

.. exercise:: Sub-group MatMul

   We will modify the tiled MatMul exercise in
   :ref:`task-graphs-synchronization` to use sub-groups instead of local memory.

   **Don't do this at home, use optimized BLAS!**

   You can find a scaffold for the code in the
   ``content/code/day-2/05_sub_group-matmul/sub_group-matmul.cpp`` file,
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

      .. code:: c++=

         host_accessor C{bufC};

      and check that your results are correct.


.. keypoints::

   .. todo::

      WRITE ME!


.. rubric:: Footnotes

.. [*] Consuld the SYCL 2020 standard for a complete list of `group-collective
       functions
       <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:group-functions>`_
       and `group-collective algorithms
       <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:algorithms>`_
