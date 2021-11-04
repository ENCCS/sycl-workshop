.. _buffer-accessor-vs-usm:

Buffer-accessor model *vs* unified shared memory
================================================

.. questions::

   - When should I use the buffer-accessor model and when is |USM| more appropriate?

.. objectives::

   - Learn the trade-offs of the buffer-accessor and |USM| models.


.. todo::

   - Here we have a rather large section all in breakout rooms where we port the
     heat equation mini-app.
   - Should we mention the interoperability of the two models in hipSYCL_?
     https://hipsycl.github.io/hipsycl/extension/hipsycl-091-buffer-usm-interop/

.. keypoints::

   - USM might bring better performance and more opportunities for fine-grained
     performance tuning.
