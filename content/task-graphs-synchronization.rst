.. _task-graphs-synchronization:

The task graph: data, dependencies, synchronization
===================================================

.. questions::

   - How can you express dependencies between SYCL_ parallel kernels?

.. objectives::

   - Learn how the runtime can handle data movements for us.
   - Learn how to work with events to fine-tune dependencies in the task graph.
   - Learn about work-group- and work-item-level synchronization.


It should be clear by now that the essential role of the runtime in SYCL
applications is to schedule work submitted to the queues in our program as
actions and execute all enqueued work in an asynchronous fashion.
How is this achieved? In episode :ref:`queues-cgs-kernels`, we briefly mentioned
the **task graph** is the underlying abstraction for this purpose in the SYCL
execution model.
A task graph is composed of *nodes* and *edges*, it has a start-to-finish
direction, and no self-loops: it is a *directed acyclic graph (DAG)*. Each node
is an action to be performed on a device, such as a ``parallel_for``. Each edge
connecting two nodes is a dependency between the two, such as the data for task
B produced by task A.

It is not usually necessary to *directly* define the task graph for your
application.

- If you use buffers and accessors, the runtime can correctly generate the task
  graph based on their use in kernel code.
- Similarly when you use USM with implicit data movement.
- For USM with explicit data movements, you are taking up scheduling power. [*]_

However, interacting with the task graph might be necessary to get performance.
In this episode, we will explain how to do so.

In-order and out-of-order queues
--------------------------------

The following figure shows some plausible task graphs in an application, *i.e.*
chains of dependencies between tasks.

.. figure:: img/graphs.svg
   :align: center

   Examples of a linear chain (left) and Y-pattern (right) of dependencies.
   The Y-pattern might occur, for example, in the AXPY routine: tasks A and B
   fill the operand vectors, thus are able to run independently, and task C
   performs the summation. Task D might be a subsequent reduction.
   We can always "linearize" a Y-pattern, for example, by using a in-order
   queue. Figure adapted from :cite:`Reinders2021-yx`.


The first interaction with the task graph happens already at queue construction.
The SYCL standard defines two queue flavors: **in-order** and **out-of-order**.

Out-of-order queues
  This is the default for ``queue`` objects and leaves all decisions on task
  ordering to the runtime, unless we intervene. The runtime will decide ordering
  based on the data dependencies declared in our code. This can be influenced
  with various mechanisms which we will discuss shortly.
In-order queues
  Each action is assumed to be dependent on the action immediately preceding it
  in the queue.  In-order queues are quite simple to reason about, but they will
  constrain the runtime too much.  Tasks will be serialized in a linear chain,
  even though they do not read/write to the same data. For this reason, in-order
  semantics is not the default, but must be set explicitly at construction:

  .. code:: c++

     queue ioQ{property::queue::in_order()};


How to specify dependencies
---------------------------

A *command group handler* can contain three things only: host code, **exactly
one** action, and specifications of action dependencies.
There are three available methods to express the latter:

#. When using **in-order** queues, each action is implicitly dependent on the
   action immediately preceding it. Successive tasks must complete strictly in
   the order in which they were submitted to the queue. This is the simplest way
   to express a *linear* chain on dependencies.
   In-order queues can be excessively constraining. For example, independent
   computations cannot be overlapped.
#. Through accessors on buffers. In episode :ref:`buffers-accessors`, we saw
   that accessors are constructed with a mode and a target. This metadata is
   enough for the runtime to infer dependencies between different kernels
   accessing the same buffers.
#. The event-based approach is the most flexible. Methods on the ``handler``
   class or on the ``queue`` class are asynchronous and return immediately
   objects of the ``event`` class. We can pass an event (or a vector of events)
   to the ``depends_on`` method on the ``handler`` class or to ``parallel_for``
   invocations on the ``queue`` class.


.. typealong:: Expressing the Y-pattern

   .. tabs::

      .. tab:: Using events

         Methods on ``queue`` or ``handler`` objects are asynchronous but return
         ``event`` objects immediately.  With events, we have granular control
         over dependencies, since they can be passed as arguments to
         ``parallel_for`` and ``single_task`` invocations and also to the
         ``depends_on`` method of the ``handler`` class.

         .. literalinclude:: code/snippets/y-pattern-events.cpp
            :language: c++
            :lines: 23-43

      .. tab:: Using accessors

         Accessors on buffers implicitly define dependencies between tasks, but
         can be slightly more verbose.

         .. literalinclude:: code/snippets/y-pattern-accessors.cpp
            :language: c++
            :lines: 27-57

      .. tab:: Using an in-order queue

         An in-order queue executes tasks in the exact order in which they were
         enqueued. Task A and B, which are independent in the Y pattern, are
         executed in sequence.

         .. literalinclude:: code/snippets/y-pattern-in-order.cpp
            :language: c++
            :lines: 21-43


Synchronization with the host
-----------------------------

We have seen how to handle dependencies between tasks to be run on devices,
let's talk about what happens on the host. Once our device computations are
done, we'd obviously like to get the results back on the host. In CUDA/HIP this
usually takes the form of device-to-host copies. These represent implicit
synchronization points between host and device: we wait until all kernels have
completed and then perform the copy. In SYCL, we have few options:

#. We can use the ``wait`` method on the ``queue`` object. Even though this has
   been used extensively in our example, it is also the **coarsest**
   synchronization level and might not be a good idea in larger-scale
   applications.
   We might submit *many* actions to a queue and using ``wait`` will *block*
   execution until *each and every one of them* has completed, which is clearly
   not always ideal.
#. For finer control, you can synchronize on events: either a single one or a list.

   .. code:: c++

      // waiting on a single event

      auto e1 = Q.parallel_for(...);
      e1.wait();

      // waiting on multiple events
      auto e2 = Q.parallel_for(...);
      auto e3 = Q.single_task(...);
      event::wait({e2, e3});
#. Use of objects of ``host_accessor`` type sits at an even finer level. They
   define a new dependency between a task in the graph and the host, such that
   execution cannot proceed past their construction until the data they access
   is available on the host. More concisely, construction of an
   ``host_accessor`` is blocking.

   .. code:: c++

      // declare buffer
      buffer<double> A{range{256}};

      // fill with ones
      Q.submit([&](handler &cgh){
        accessor aA{A, cgh};
        cgh.parallel_for(range{N}, [=](id<1> id){
          aA[id] = 1.0;
        });
      });

      // enqueue more work

      // host accessor for buffer A
      // the constructor will *block* until data is available on host
      host_accessor h_a{A};

   Note that the same data accessed by an host accessor **cannot** be "touched"
   on the device as long as the ``host_accessor`` object exists.
   We can achieve the same behavior just with buffers. We have seen that buffer
   destructors are also *blocking*: when a buffer goes out of scope, it will
   implicitly wait for all actions that use it to complete. If the buffer was
   initialized with a host pointer, [*]_ then the runtime will schedule a copy back
   to the host:

   .. code:: c++

      std::vector a(256, 0.0);

      { // open scope
        // buffer to a
        buffer<double> buf_a(a.data(), range{256});

        // use buffer in work submitted to the queue
        Q.submit([&](handler &cgh){
          auto acc_a = accessor(buf_a, cgh);

          cgh.parallel_for(...);
        });
      } // close scope: buffer destructor will wait and host data will be updated


Kernel-level communication
--------------------------

.. todo::

   chapter 9 "Communication and synchronization"

   - Group collectives: tiled matmul as exercise


.. exercise:: Tiled MatMul

   We can further optimize the ND-range implementation of matrix multiplication
   by using *tiling*. The basic idea is to exploit the fact that each row of the
   left operand :math:`\mathbf{A}` is reused multiple times to compute elements
   in the result. If we can structure the local iteration range to work over
   tiles (subsections) of it, we can achieve better locality.

   Each work-item will compute an element in the result matrix
   :math:`\mathbf{C}` by loading a **tile** (subsection) of a row of
   :math:`\mathbf{A}` into work-group local memory and multiplying it with an
   appropriately sized portion of columns of :math:`\mathbf{B}`.  The result
   matrix is held in global memory and addressed through the global range of the
   ND-range object. Local memory accesses should be faster and each tile is
   reused multiple times.

   .. figure:: img/tiled_matmul.svg
      :align: center

      Schematics of a *tiled* implementation of matrix multiplication:
      :math:`C_{ij} = \sum_{k}A_{ik}B_{kj}`. The computation is split into
      work-groups, each with own local memory. We first load a **tile** (cyan)
      of the left operand matrix :math:`\mathbf{A}` into local memory. The tile
      will be reused multiple times by each work-item to compute the result
      (green), held in global memory.  The right operand matrix
      :math:`\mathbf{B}` is also accessed from global memory.
      Figure adapted from :cite:`Reinders2021-yx`.

   ***Don't do this at home, use optimized BLAS!**

   You can find a scaffold for the code in the
   ``content/code/day-2/02_nd_range-matmul/nd_range-matmul.cpp`` file,
   alongside the CMake script to build the executable. You will have to complete
   the source code to compile and run correctly: follow the hints in the source
   file.  A working solution is in the ``solution`` subfolder.

   #. We create a queue and map it to the GPU.
   #. We declare the operands as ``std::vector<double>``. Generalize the
      example in the previous exercise to allow multiplication of non-square
      matrices. the right-hand side operands are filled with random numbers,
      while the result matrix is zeroed out.
   #. We define buffers to the operands in our matrix multiplication.
   #. We submit work to the queue through a command group handler.
   #. We set up accessors for the matrix buffers. We can use access targets and
      properties to guide the runtime in the creation of the task graph.
   #. We also have to set up a local accessor to our tiles. Note that the tile
      is a 1-dimensional range.
   #. Within the handler, we launch a ``parallel_for`` with an appropriately
      sized ``nd_range`` execution range.  The tile will be our local range and
      it is 1-dimensional.
   #. Define the matrix multiplication kernel function, where we have:

      - A tile-strided loop to load data for :math:`\mathbf{A}` from global to
        local memory.
      - A loop over work-items in the tile to compute their product with
        :math:`\mathbf{B}`.

      Remember that we need to ensure that the local memory is *consistent*
      across work-items after every load and/or store operation!
   #. Check that your results are correct.


.. keypoints::

   - The SYCL *task graph* is built by the runtime and governs execution of our
     program on heterogeneous hardware.
   - Data dependencies are the main ingredients in the task graph construction.
   - We can influence the task graph explicitly through events.
   - In a data-parallel kernel, work-items within a work-group can cooperate and
     we can leverage this to our advantage.

.. rubric:: Footnotes

.. [*] And with great power, comes great responsibility.
.. [*] One could also set the host pointer *after* buffer construction using the ``set_final_data`` method.
