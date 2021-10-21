.. _device-discovery:

Device discovery
================

.. questions::

   - How can we make SYCL_ aware of the available hardware?

.. objectives::

   - Learn how to query platform and/or device information.
   - Learn to use device selectors.


.. todo::

   How devices are indexed and handled


.. challenge:: Compiling for different targets

   SYCL_ is all about being able to write code *once* and execute it on
   different hardware. It is quite simple to achieve this with hipSYCL_: when
   configuring use a different argument for the ``HIPSYCL_TARGETS`` CMake
   option.

   Take the previous example and:

   1. Compile to target the GPU, using ``-DHIPSYCL_TARGETS="cuda:sm_80"`` in the
      configuration step.  What output do you see? Is the code running on the
      device you expect?
   2. Compile to target the GPU *and* the host device using OpenMP with
      ``-DHIPSYCL_TARGETS="cuda:sm_80;omp"``. What output do you see now?

   To learn more about the compilation model in hipSYCL_, check out `its
   documentation
   <https://github.com/illuhad/hipSYCL/blob/develop/doc/compilation.md>`_.




.. challenge:: Finding your devices

   .. todo::

      write me!


.. keypoints::

   .. todo::

      Write me
