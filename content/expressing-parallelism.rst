.. _expressing-parallelism:

Expressing parallelism with SYCL
================================

.. questions::

   - How do we write parallel kernels in SYCL_?

.. objectives::

   - Learn the difference between *task* and *data* parallelism.
   - Learn about *work-items*, *work-groups*, and *nd-ranges*.

We can submit a single task for sequential execution with ``single_task``.

.. todo::

   Maybe a type-along with ``single_task``?


Data-parallel kernels come in three flavours:

- simple
- ND-range
- hierarchical

We will not discuss hierarchical parallel kernels. This is a fast-changing area
in the standard: support in various implementation varies and may lack support
for essential features or underperform.

Simple data-parallel kernels
----------------------------

.. figure:: img/2d_range.svg
   :align: center

   A 2-dimensional range.

.. challenge:: Naïve MatMul

   .. figure:: img/naive_matmul.svg
      :align: center

      Naïve MatMul **don't do this at home, use optimized BLAS!**

   .. todo::

      Write a data-parallel kernel to multiply two matrices.


ND-range kernels
----------------

.. figure:: img/nd_range.svg
   :align: center
   :scale: 50%

   ND-range.


.. challenge:: Less naïve MatMul

   .. figure:: img/less_naive_matmul.svg
      :align: center

      Naïve MatMul **don't do this at home, use optimized BLAS!**

   .. todo::

      Write a data-parallel kernel to multiply two matrices using ND-range.



.. keypoints::

   .. todo::

      Write me
