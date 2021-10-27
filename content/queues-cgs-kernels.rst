.. _queues-cgs-kernels:

Queues, command groups, and kernels
===================================

.. questions::

   - How do we express parallelism in SYCL?

.. objectives::

   - Learn about queues to describe ordering of operations.
   - Command groups.
   - Understand that kernels are units of parallelism in SYCL.


.. todo::

   Write an intro

As we mentioned the queues connect a host program to a single device. Then
the program submit tasks to a device via queue and it could monitor the queue
until it is completed. The task is initiated by submitting a command group to 
a queue. A command group defines a kernel function, the requirements to execute 
the kernel function, and its index space.

The runtime implements many components such as device management, task graph management, 
execution, data management, backend management, scheduling and task synchronization. 


   - Relation between queues, command groups, and kernels.
   - SYCL runtime and implicit scheduling.


The :term:`queue` is a central abstraction in SYCL_. All device code is
**submitted** to a queue as *actions*:

.. code:: c++

   queue Q;

   Q.submit(
     /* device code action */
   );

the runtime **schedules** the actions and executes them **asynchronously**.

One queue maps to one device: the mapping happens upon construction of a
``queue`` object and cannot be changed subsequently.
It is not possible to use a single ``queue`` object to:
- manage more than one device, as this would be ambiguous for the runtime to
  decide which device should actually do the work that has been enqueued.
- spead enqueued work over multiple devices.

While these might appear as limitations, we are free to declare as many
``queue`` object as we like in our program. It is also valid to create multiple
queues to the *same* device.



.. keypoints::

   .. todo::

      Write me
