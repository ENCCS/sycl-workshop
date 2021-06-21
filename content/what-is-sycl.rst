.. _what-is-sycl:

What is SYCL?
=============

.. questions::

   - What is SYCL and why should I care?

.. objectives::

   - Learn how to write a simple C++ SYCL code.
   - Learn how to compile with SYCL enabled.
   - Learn about device discovery in SYCL code.


.. todo::

   Write an intro

   - Computing platforms are and will be powered by heterogeneous hardware.
   - Standards-based, vendor-agnostic programming for heterogeneous hardware
   - Compilation model
   - Evolution in relation to C++ standard (Khronos group has a chart, I think)


Hello, SYCL!
------------

.. todo::

   Highlight queues, command groups, kernels.

.. typealong:: "Hello world" with SYCL

   Download  :download:`scaffold project <code/tarballs/00_hello-cxx.tar.bz2>`.

   .. literalinclude:: code/day-1/00_hello-cxx/hello.cpp
      :language: c++

   You can download the :download:`complete, working example <code/tarballs/00_hello-cxx_solution.tar.bz2>`.

   Then unpack the archive::

     tar xf hello-cxx_solution.tar.bz2


Device discovery
----------------

.. todo::

   How devices are indexed and handled


.. challenge:: Finding your devices

   Download the :download:`scaffold project <code/tarballs/01_libraries-cxx.tar.bz2>`.

   #. Write a ``CMakeLists.txt`` to compile the source files
      ``Message.hpp`` and  ``Message.cpp`` into a library. Do not specify
      the type of library, shared or static, explicitly.
   #. Add an executable from the ``hello-world.cpp`` source file.
   #. Link the library into the executable.

   You can download the :download:`complete, working example <code/tarballs/01_libraries-cxx_solution.tar.bz2>`.


.. keypoints::

   - SYCL is an *open-source standard* to describe parallelism on heterogeneous
     platforms using the C++ programming language.
   - There are many implementations of the SYCL standard: Intel DPC++, hipSYCL,
     CodePlay ComputeCPP.
