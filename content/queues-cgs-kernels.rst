.. _queues-cgs-kernels:

Queues, command groups, and kernels
===================================

.. questions::

   - How do we express parallelism in SYCL?

.. objectives::

   - Learn about queues to describe ordering of operations.
   - Command groups.
   - Understand that kernels are units of parallelism in SYCL.


SYCL ``queue`` objects are the abstraction connecting a host program to a single
device.  The :term:`queue` is a central abstraction in SYCL_: all device code is
**submitted** to a queue as *actions*. The runtime **schedules** the actions and
executes them **asynchronously**.  The runtime keeps track of action
prerequisites in its scheduling, for example, availability of data.
We can state that the tracking of actions and their dependencies is the essence
of SYCL_.  The SYCL standard models our program as a **task graph**, a set of
*nodes* connected by *edges*:

- **Nodes** are actions to be performed on a device, such as the invocation of a
  kernel or explicit data movements.
- **Edges** are dependencies between the actions and express when it's legal for
  a node to execute. Edges arise most often because of data dependencies between
  nodes.

The task graph is a *directed acyclic graph (DAG)*: it has a definite
start-to-finish direction and no nodes are self-connected.
The SYCL runtime can resolve dependencies and thus **generate** the task graph.
Furthermore, it can schedule how to execute the nodes, *i.e.* **traversal** of
the task graph, in a completely asynchronous manner from the execution of the
host code.
We will see in :ref:`task-graphs-synchronization` how to manually modify the
task graph.

Two kinds of actions can be part of the task graph:

Execution of device code
   These actions add nodes to the graph that will, eventually, execute device
   code. They accept kernel code and its execution space as argument and you
   invoke them as methods on the ``queue`` class directly or on the ``handler``
   class. They come in three flavors, which represent different abstractions for
   work distribution in SYCL:

   - ``single_task``: as the name says, this will execute one single instance of
     the kernel code.
   - ``parallel_for``: this will launch a kernel with given work-size
     specification in a single instruction, multiple threads (SIMT) fashion.
   - ``parallel_for_work_group``: launches a kernel with hierarchical
     parallelism. This is only available on the ``handler`` class.

Explicit memory operations.
   These actions add nodes to the graph that will, eventually, perform data migrations.
   You invoke them as methods on the ``queue`` class directly or on the ``handler``
   class:

   - ``copy``: copies data.
   - ``update_host``: updates data in the buffer on the host-side.
   - ``fill``: initializes data in a buffer to the given value.

We have given a high-level overview of the abstractions in the execution model:
from the queue to the execution on a device, passing through submission of work,
described as a data-parallel kernel.

But how do we write a kernel?

Kernels
-------

Kernels are the fundamental building blocks for performing work in a SYCL
program. We will only consider two ways of writing kernels in SYCL:

`lambda expressions <https://en.cppreference.com/w/cpp/language/lambda>`_
  Kernels as lambdas are very concise, thanks especially to the *capture*
  syntax. They cannot be templated and might be cumbersome to reuse. In some
  cases, lambdas can be too terse.

  .. demo:: +1 as a lambda

     .. code:: c++

        [=](id<1> idx) -> void {
          data_acc[idx] += 1;
        }


`function objects <https://en.cppreference.com/w/cpp/utility/functional>`_
  A kernel is a class that overloads ``operator()`` function call operator. They
  can be templated, easily reused, and give full control over what data is
  passed in and out.  They are more verbosee.

  .. demo:: +1 as a function object

     .. code:: c++

        class PlusOne {
          public:
           PlusOne(accessor<int> acc) : data_acc_(acc) {}

           void operator()(id<1> idx) {
             data_acc[idx] += 1;
           }

          private:
           accessor<int> data_acc_;
        };


There are no technical reasons to prefer one style over the other, it will ultimately boil down to personal preference. Regardless of the chosen style, kernel code has some restrictions:

- It must have ``void`` as return type.
- It cannot use `runtime type identification (RTTI) <https://en.m.wikibooks.org/wiki/C%2B%2B_Programming/RTTI>`_.
- It cannot dynamic allocate memory.


Queues
------

One queue maps to one device: the mapping happens upon construction of a
``queue`` object and cannot be changed subsequently.
It is not possible to use a single ``queue`` object to:

- manage more than one device. The runtime would face ambiguities in deciding
  which device should actually do the work!
- spread enqueued work over multiple devices.

While these might appear as limitations, we are free to declare as many
``queue`` object as we like in our programs. It is also valid to create multiple
queues to the *same* device.  Thus, the relation between queues and devices is
**many-to-one**.

Work on a device can be enqueued with the shortcut methods described above. For
example, we can launch a data-parallel kernel with ``parallel_for``  invoked on
the desired queue object:

.. demo:: Creating work on a device using ``queue`` shortcuts.

   .. code:: c++

      auto Q = queue{my_selector{}};

      Q.parallel_for(range<1>{sz}, [=](auto &idx){
        /* kernel code */
      });


Command groups
--------------

A command group handler gives more control over how code is submitted to the
queue. Submission is slightly more verbose, but we get access to features of
hierarchical parallelism.
The abstraction for command groups is the class ``handler``: these objects are
constructed for us by the SYCL runtime.  As such, we will meet them only as
arguments of the lambda functions passed to the ``submit`` method of our queues.
A command group handler contains:

- host code, to set up the dependencies of the corresponding node in the task graph.
  Host code is executed immediately upon submission.
- **exactly one** action of the ones described above. The action executes
  asynchronously on the device.  Parallel work actions will, furthermore, need
  an execution range and a kernel function.

.. demo:: Creating work on a device using a command group ``handler``.

   .. code:: c++

      auto Q = queue{my_selector{}};

      Q.submit([&](handler &cgh){
       /* host code: sets up the dependencies of this node. It executes **immediately!** */
       accessor acc{B, h};

       /* exactly **one** of the available actions. It executes **asynchronously** */
       cgh.parallel_for(range<1>{sz}, [=](auto &idx){
          /* kernel code */
       });
      });


.. exercise:: ``single_task`` and streams

   We'll walk through the use of the ``single_task`` method to create work on a
   device.
   As the name suggests, this will create a task for sequential execution:
   probably not a method you will use often, but definitely something to be
   aware of!
   The task we would like to perform is a print-out on the device. If you are
   familiar with CUDA/HIP, you probably know that ``printf`` can be used in
   device code. In keeping with C++, the SYCL standard defines a ``stream``
   class, which works similar to the standard streams. A SYCL stream needs a ``handler`` object on construction:

   .. code:: c++

      auto out = stream(1024, /* maximum size of output per kernel invocation */
                         256, /* maximum size before flushing the stream */
                         cgh);

   You can find a scaffold for the code in the
   ``content/code/day-1/04_single-task/single-task.cpp`` file,
   alongside the CMake script to build the executable. You will have to complete
   the source code to compile and run correctly: follow the hints in the source
   file.  A working solution is in the ``solution`` subfolder.

   #. Create a queue object. You're free to use any of the device selection
      strategies we have encountered in the previous episode.
   #. Submit work to the queue using a command handler group.
   #. Create a ``stream`` object.
   #. Create a single task on the ``handler`` printing a string to the stream.


.. keypoints::

   - One queue maps to one device, such that there is no ambiguity in
     spreading work.
   - A program can have as many queues as desired. Multiple queues can use the
     same device: the queue-device mapping is many-to-one.
   - Enqueing actions can happen by submitting **command groups** using the ``handler`` class.
   - You can also enqueue actions with *shortcut* methods on the ``queue`` class.
   - Work can be enqueued with a command group handler. This gives more
     flexibility over the definition of the corresponding node in the task
     graph.
   - Kernels are `callables
     <https://en.cppreference.com/w/cpp/named_req/Callable>`_: either lambda
     functions or function objects.
   - Kernel code cannot use neither RTTI nor dynamic memory allocation.
