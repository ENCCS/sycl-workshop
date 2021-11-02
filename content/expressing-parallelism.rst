.. _expressing-parallelism:

Expressing parallelism with SYCL
================================

.. questions::

   - How do we write parallel kernels in SYCL_?

.. objectives::

   - Learn the difference between *task* and *data* parallelism.
   - Learn about *work-items*, *work-groups*, and *nd-ranges*.


We should have acquired a good enough understanding of the anatomy of a SYCL
program: queues, kernels, and memory management strategies. We have, however,
not really discussed about strategies for *expressing parallelism* in a SYCL
program. Indeed, the kernels that we have seen and written so far in the exercises
always used a 1-dimensional work distribution, exemplified by the use of
``range<1>`` and ``id<1>`` objects as arguments of ``parallel_for`` and the
kernel lambdas, respectively.
Our particular problem domain might have additional structure and our program
might benefit, in terms of performance, from exploiting it.

Data and task parallelism
-------------------------

The are two types of parallelism that can be explored and exploited: [*]_

Data parallelism
  The data can be distributed across computational units that can run in
  parallel.  Each unit processes the data applying the same or very similar
  operations to different data elements.  A common example is applying a blur
  filter to an image: the same function is applied to *all* the pixels the
  image is made of.  This parallelism is natural for the GPU, where the same
  instruction set is executed in multiple threads.
Task parallelism
  When an application consists of many tasks that perform different operations
  with (the same or) different data. An example of task parallelism is cooking:
  slicing vegetables and grilling are very different tasks and can be done at
  the same time, if more cooks are available.  Note that the tasks can consume
  totally different resources, for example a CPU and a GPU, adding yet another
  dimension for performance optimization.

.. figure:: img/ENCCS-OpenACC-CUDA_TaskParallelism_Explanation.png
    :align: center
    :scale: 40 %

    Data parallelism and task parallelism.  We have data parallelism when the
    same operation applies to multiple data, *e.g.* multiple elements of an array
    are transformed. Task parallelism implies that there are more than one
    independent task that, in principle, can be executed in parallel.

In SYCL, we can largely delegate task parallelism to the runtime scheduler:
actions on queues will be arranged in a DAG and tasks that are independent *may*
execute in an overlapping fashion. Episode :ref:`task-graphs-synchronization`
will show how to gain more direct control.
The ``parallel_for`` function is the basic data-parallel construct in SYCL and it accepts two arguments: an *execution range* and a *kernel* function.
Based on the way we specify the execution range, we can distinguish three
flavors of data-parallel kernels in SYCL:

- basic: when execution is parameterized over a 1-, 2- or 3-dimensional
  ``range`` object. As the name suggests, simple kernels do not provide control
  over low-level features, for example, control over the locality of memory
  accesses.

  .. signature:: simple ``parallel_for``

     .. code:: c++

        template <typename KernelName, int Dims,
             typename... Rest>
        event parallel_for(range<Dims> numWorkItems,
           Rest&&... rest);

- ND-range: when execution is parameterized over a 1-, 2-, or 3-dimensional
  ``nd_range`` object. While superficially similar to simple kernels, the use of
  ``nd_range`` will allow you to group instances of the kernel together. You
  will gain more flexible control over locality and mapping to hardware
  resources.

  .. signature:: ND-range ``parallel_for``

     .. code:: c++

        template <typename KernelName, int Dims,
             typename... Rest>
        event parallel_for(nd_range<Dims> executionRange,
           Rest&&... rest);

- hierarchical: these form allows to *nest* kernel invocations, affording some
  simplifications with respect to ND-range kernels. We will not discuss
  hierarchical parallel kernels. This is a fast-changing area in the standard:
  support in various implementation varies and may not work properly.

.. note::

   A data-parallel kernel **is not** a loop. When we launch a kernel through a
   ``parallel_for``, the runtime will instantiate the same operation over all
   data elements in the execution range. These instances run in parallel (but
   not necessarily in lockstep!) As such, a data-parallel kernel is not
   performing any iterations and should be viewed as a *descriptive abstraction*
   on top of the execution model and the backend.


Basic data-parallel kernels
---------------------------

Basic data-parallel kernels are the most suited for **embarassing data
parallelism**, such as the blurring filter example above.  The first argument to
the ``parallel_for`` invocation is the execution range, represented by a
``range`` object in 1-, 2-, or 3-dimensions. A ``range`` is a grid of objects of
type ``item``, each ``item`` is an instance of the kernel and is uniquely
addressable through object of ``id`` type. :math:`N`-dimensional ranges are
arranged in *row-major order*: dimension :math:`N-1` is contiguous.

.. figure:: img/2d_range.svg
   :align: center
   :scale: 60%

   A ``range<2>`` object, representing a 2-dimensional execution range. Each
   element in the range is of type ``item<2>`` and is indexed by an object of
   type ``id<2>``. Items are instances of the kernel. An :math:`N`-dimensional
   range is in row-major order: dimension :math:`N-1` is contiguous.

In basic data-parallel kernels, the kernel function passed as second argument to
the ``parallel_for`` invocation can accept either objects of ``id`` or of
``item`` type.
In all cases, the ``range`` class describes the sizes of *both* buffers **and**
execution range of the kernel.
When to use ``id`` and when to use ``item`` as arguments in the kernel function?

.. demo:: ``id`` knows about the individual kernel instance only.

   In this kernel, we set all elements in a 2-dimensional array to the magic
   value of 42. This is embarrassingly parallel and each instance of the kernel
   only needs access to one element in the buffer, indexed by the ``id`` of the
   instance.

   .. literalinclude:: code/snippets/kernel_with_id.cpp
      :language: c++

.. demo:: ``item`` knows about the individual kernel instance *and* the global execution range.

   In this kernel, we sum two vectors using a 1-dimensional execution range.
   Passing ``item<1>`` as argument to the kernel lets us probe the global index
   of the individual kernel instance in the ``parallel_for``. We use it to index
   our accessors to the buffers.

   .. literalinclude:: code/snippets/kernel_with_item.cpp
      :language: c++



.. exercise:: Naïve MatMul

   Let's now write a data-parallel kernel of the basis flavor to perform a
   matrix multiplication.

   .. figure:: img/naive_matmul.svg
      :align: center

      Schematics of a naïve implementation of matrix multiplication:
      :math:`C_{ij} = \sum_{k}A_{ik}B_{kj}`. Each kernel instance will compute
      an element in the result matrix :math:`\mathbf{C}` by accessing a full row
      of :math:`\mathbf{A}` and a full column of :math:`\mathbf{B}`.


   **Don't do this at home, use optimized BLAS!**


ND-range kernels
----------------

.. figure:: img/nd_range.svg
   :align: center
   :scale: 50%

   ND-range.


.. exercise:: Less naïve MatMul

   Using the ND-range flavor of data-parallelism should let us optimize memory
   accesses a bit more.  In this exercise, we will rewrite the matrix
   multiplication kernel to use ``nd_range``.

   .. figure:: img/less_naive_matmul.svg
      :align: center

      Schematics of a naïve implementation of matrix multiplication:
      :math:`C_{ij} = \sum_{k}A_{ik}B_{kj}`. Each kernel instance will compute
      an element in the result matrix :math:`\mathbf{C}` by accessing a full row
      of :math:`\mathbf{A}` and a full column of :math:`\mathbf{B}`.

   **Don't do this at home, use optimized BLAS!**


.. keypoints::

   - The task graph abstraction in SYCL can take care of task parallelism for us.
   - Data parallelism is achieved with ``parallel_for`` and kernel-based programming.
   - There are three flavors of data-parallel kernels. The basic and ND-range
     forms are stable in SYCL 2020.
   - Basic kernels are especially well-suited for embarassing parallelism.
   - ND-range kernels should be used for more sophisticated control over
     performance aspects.


.. rubric:: Footnotes

.. [*] This section is adapted, with permission, from the training material for the `ENCCS CUDA workshop <https://enccs.github.io/CUDA/1.01_GPUIntroduction/#exposing-parallelism>`_.
