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

   .. todo::

      WRITE INTRO!

   **Don't do this at home, use optimized BLAS!**

   .. tabs::

      .. tab:: Using buffers and accessors

         .. todo::

            CREATE SCAFFOLD!

         You can find a scaffold for the code in the
         ``content/code/day-2/05_sub_group-matmul/sub_group-matmul.cpp`` file,
         alongside the CMake script to build the executable. You will have to complete
         the source code to compile and run correctly: follow the hints in the source
         file.  A working solution is in the ``solution`` subfolder.

         #. We first declare the operands as ``std::vector<double>`` the
            right-hand side operands are filled with random numbers, while the
            result matrix is zeroed out.
         #. We create a queue and map it to the GPU.
         #. We define buffers to the operands in our matrix multiplication.
         #. We submit work to the queue through a command group handler.
         #. Within the handler, we launch a ``parallel_for``.
         #. Check that your results are correct.

      .. tab:: Using USM

        .. todo::

           WRITE USM CODE!

         You can find a scaffold for the code in the
         ``content/code/day-2/03_usm-nd_range-matmul/usm-nd_range-matmul.cpp`` file,
         alongside the CMake script to build the executable. You will have to complete
         the source code to compile and run correctly: follow the hints in the source
         file.  A working solution is in the ``solution`` subfolder.

         #. We first create a queue and map it to the GPU.
         #. We allocate the operands as USM buffers and fill them with random
            numbers. We can do this with untyped or typed ``malloc``-style or
            ``usm_allocator`` APIs. Should operands be host, device, or shared
            allocations?
         #. We allocate the result as USM buffer and zero it out.  We can do
            this with untyped or typed ``malloc``-style or ``usm_allocator``
            APIs. Should this be host, device, or shared allocation?
         #. We submit work to the queue. Note that we need to linearize indices
            for row-major access to our buffers!
         #. Check that your results are correct.


.. keypoints::

   .. todo::

      WRITE ME!


.. rubric:: Footnotes

.. [*] Consuld the SYCL 2020 standard for a complete list of `group-collective
       functions
       <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:group-functions>`_
       and `group-collective algorithms
       <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:algorithms>`_
