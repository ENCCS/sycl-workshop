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

   - Implicit memory management with buffers and accessors.
   - Dependencies in relation with the scheduling done by the SYCL runtime.



.. keypoints::

   - Buffers and accessors delegate memory management issues to the SYCL runtime.
   - SYCL lets you abstract away the intricacies of host-device data dependencies.
   - It can be hard to adapt an existing code to the buffer-accessor model.
   - There might be performance overhead when adopting the buffer-accessor model.
