.. _buffers-accessors:


Data management with buffers and accessors
==========================================

.. questions::

   - What facilities does SYCL offer to manage memory spaces in an heterogeneous environment?

.. objectives::

   - Learn about the buffer and accessor API.
   - Learn how the SYCL runtime manages memory.


.. todo::

   Write an intro

In SYCL we have three abstractions for managing memory: Unified Shared Memory (USM),
buffers, and images. In this section we discuss about buffers. The buffer template class 
can describe up to 3-dimentional arrays. An abstract view of memory is provided, available to
access from host or device. However, a buffer, which can be a familiar term to programmers, can
not be accessed directly. An accessor object is required to access a buffer, as it is constituted
by data objects. There are three access modes, read, write, and read_write modes. 
We can define implicit dependencies between accessors in order to have a specific flow in our
program.

   - Implicit memory management with buffers and accessors.
   - Dependencies in relation with the scheduling done by the SYCL runtime.



.. keypoints::

   - Buffers and accessors delegate memory management issues to the SYCL runtime.
   - SYCL lets you abstract away the intricacies of host-device data dependencies.
   - It can be hard to adapt an existing code to the buffer-accessor model.
   - There might be performance overhead when adopting the buffer-accessor model.
