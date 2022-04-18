.. _profiling:

Profiling SYCL applications
===========================

.. questions::

   - How can we identify performance issues in our SYCL_ code?

.. objectives::

   - Learn to use the profiling interface in the SYCL_ standard.
   - Explore graphical profilers.

It is rarely the case that the first version of a performance-critical code
delivers satisfactory performance.
Creating performant software requires awareness of the problem domain and the
algorithms employed, but a good dose of empirical performance measurements are
also needed to remove bottlenecks and optimize the code.
Time-to-solution is an important performance metrics, but it's not the sole
metric one should care about: how well are we using the hardware at hand? Are
the available cores always busy? What about memory usage?
Performance profiling helps us answer these questions and guides us in
identifying and fixing performance issues.

This episode will show, using the heat equation mini-app, how to obtain this
kind of information when SYCL_ is our programming model of choice.


Profiling with SYCL events [*]_
-------------------------------


https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:interface.event

https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#table.event.profilinginfo

Profiling with backend tools
----------------------------

The use of SYCL_ events is not the best way to go about profiling:

#. The information provided might not be as fine-grained as needed to correctly
   identify performance bottlenecks.
#. The changes needed to adapt an existing code to "profiling" mode might be too
   substantial.

It is usual to rely on *sampling profilers* to obtain a more well-rounded
snapshot of the performance aspects of a software. Sampling profilers accumulate
statistical measures of given *counters*, such as memory loads/stores and
register occupancy, and provide a final report to the user. They introduce
execution overhead, but the wealth of information they can provide is extremely
valuable.

There are *many* performance profiling tools available, here is an incomplete list:

- `Intel VTune`_ which offers both command-line tools and a GUI to obtain and
  analyze performance on x86 architectures, especially suited for Intel CPUs.
- `AMD uprof`_ similar to VTune, but for AMD CPUs.
- `NVIDIA Nsight Systems`_ command-line tools and GUI for performance profiling
  of CUDA GPU code.
- `AMD rocprof`_ a command-line tool for performance profiling of ROCm and HIP
  GPU code.
- `LIKWID`_ a free, open-source set of command-line tools that works on many CPU
  architectures and NVIDIA GPUs.

When we tried to answer the question :ref:`what-is-sycl` we saw that the
implementors of the standard rely on existing low-level backends for parallel
code generation.

.. figure:: img/sycl_impls+backends.svg
   :align: center

   Each of the SYCL_ implementations can map to a number of *native backends*.
   Many of these provide a full-fledged ecosystem of programming tools,
   including profilers and debuggers. Any of these can be used with compiled
   SYCL_ code.

For example, the hipSYCL_ compiler can generate GPU code targeting the CUDA and
ROCm backends and it is thus possible to use the respective profiling tools to
analyze the performance of our SYCL_ code.

.. exercise:: Profiling the SYCL heat equation mini app with `NVIDIA Nsight Systems`_

   Before starting, you should open a VNC session to Karolina, following the
   instructions in :ref:`vnc-sessions`.

   .. todo::

      WRITE ME!


.. keypoints::

   - The SYCL_ `event API`_ offers a standardized interface for
     back-of-the-envelope performance estimation.
   - You can still use the performance tools offered by your backend of choice.

.. _event API: https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:interface.event
.. _Intel VTune: https://www.intel.com/content/www/us/en/develop/documentation/vtune-help/top.html
.. _NVIDIA Nsight Systems: https://docs.nvidia.com/nsight-systems/index.html
.. _AMD uprof: https://developer.amd.com/amd-uprof/
.. _AMD rocprof: https://docs.amd.com/bundle/AMD-ROCProfiler-User-Guide/page/Introduction.html
.. _LIKWID: https://hpc.fau.de/research/tools/likwid/

.. rubric:: Footnotes

.. [*] This section of the episode was inspired by the `CodePlay tutorial <https://www.codeplay.com/portal/blogs/2019/08/27/optimizing-your-sycl-code-using-profiling.html>`_
       on profiling using the SYCL_ `event API`_.
