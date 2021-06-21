.. _unified-shared-memory:


Data management with unified shared memory
==========================================

.. questions::

   - What facilities does SYCL offer to manage memory spaces in an heterogeneous environment?

.. objectives::

   - Learn about the unified shared memory (USM) API.


.. todo::

   Write an intro

   - Pointer-based memory management.
   - How to handle data-dependencies.
   - *Future of SYCL*: USM-buffer interoperability. It is only available as an
     extension in hipSYCL:
     https://hipsycl.github.io/hipsycl/extension/hipsycl-091-buffer-usm-interop/
     But maybe in the next standard?


.. keypoints::

   - Unified shared memory (USM) provides a pointer-based API for low-level,
     explicit data management in SYCL.
   - It is useful when porting existing code to SYCL, as it might require less
     pervasive changes to the codebase.
   - USM might bring better performance and more opportunities for fine-grained
     performance tuning.
