Instructor's guide
------------------

Learning outcomes
^^^^^^^^^^^^^^^^^

- What is SYCL and why it is useful.
- Write hardware-agnostic code to express parallelism using the queue, command
  group, and kernel abstractions.
- Use buffer and accessors to handle memory across devices.
- Evaluate drawbacks and advantages of unified shared memory.
- Get acquainted with the SYCL profiling API.

Second iteration
^^^^^^^^^^^^^^^^

**Day 1 - Tuesday 19 April 2022**

.. csv-table::
   :widths: auto
   :align: center
   :delim: ;

    9:00 -  9:10 ; Welcome and introduction to the training course
    9:10 -  9:40 ; :ref:`what-is-sycl`
    9:40 -  9:45 ; Break
    9:45 - 10:15 ; :ref:`device-discovery`
   10:15 - 10:25 ; Break
   10:25 - 10:55 ; :ref:`queues-cgs-kernels`
   10:55 - 11:00 ; Break
   11:00 - 11:30 ; :ref:`buffers-accessors`
   11:30 - 11:35 ; Break
   11:35 - 12:05 ; :ref:`unified-shared-memory`
   12:05 - 12:15 ; Break
   12:15 - 12:30 ; Wrap-up

**Day 2 - Wednesday 20 April 2022**

.. csv-table::
   :widths: auto
   :align: center
   :delim: ;


    9:00 -  9:10 ; What did we cover yesterday?
    9:10 -  9:40 ; :ref:`expressing-parallelism-basic`
    9:40 -  9:50 ; Break
    9:50 - 10:20 ; :ref:`expressing-parallelism-nd-range`
   10:20 - 10:30 ; Break
   10:30 - 11:10 ; :ref:`task-graphs-synchronization`
   11:10 - 11:20 ; Break
   11:20 - 12:00 ; :ref:`heat-equation`
   12:00 - 12:10 ; Break
   12:00 - 12:30 ; Wrap-up

**Day 3 - Wednesday 21 April 2022**

.. csv-table::
   :widths: auto
   :align: center
   :delim: ;


    9:00 -  9:10 ; What did we cover yesterday?
    9:10 -  9:40 ; :ref:`sub-groups`
    9:40 -  9:50 ; Break
    9:50 - 10:20 ; :ref:`profiling`
   10:20 - 10:30 ; Break
   10:30 - 11:10 ; :ref:`buffer-accessor-vs-usm`
   11:10 - 11:20 ; Break
   11:20 - 11:30 ; Wrap-up


First iteration
^^^^^^^^^^^^^^^

**Day 1 - Monday 8 November 2021**

.. csv-table::
   :widths: auto
   :align: center
   :delim: ;

    9:00 -  9:10 ; Welcome and introduction to the training course
    9:10 -  9:40 ; :ref:`what-is-sycl`
    9:40 -  9:45 ; Break
    9:45 - 10:15 ; :ref:`device-discovery`
   10:15 - 10:25 ; Break
   10:25 - 10:55 ; :ref:`queues-cgs-kernels`
   10:55 - 11:00 ; Break
   11:00 - 11:30 ; :ref:`buffers-accessors`
   11:30 - 11:35 ; Break
   11:35 - 12:05 ; :ref:`unified-shared-memory`
   12:05 - 12:15 ; Break
   12:15 - 12:30 ; Wrap-up

**Day 2 - Tuesday 9 November 2021**

.. csv-table::
   :widths: auto
   :align: center
   :delim: ;


    9:00 -  9:10 ; What did we cover yesterday?
    9:10 -  9:50 ; :ref:`expressing-parallelism`
    9:50 - 10:00 ; Break
   10:00 - 10:40 ; :ref:`task-graphs-synchronization`
   10:40 - 10:50 ; Break
   10:50 - 11:10 ; :ref:`heat-equation`
   11:10 - 11:15 ; Break
   11:15 - 11:55 ; :ref:`buffer-accessor-vs-usm`
   11:55 - 12:00 ; Break
   12:00 - 12:30 ; Wrap-up
