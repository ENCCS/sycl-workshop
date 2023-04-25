Quick Reference
---------------

.. glossary::

   CTAD
   class template argument deduction
      Since C++17, the compiler can deduce template arguments during instantiation. For example:

      .. code:: c++

         std::vector A(10, 0.0);

      instead of the more verbose:

      .. code:: c++

         auto A = std::vector<double>(10, 0.0);

   unified shared memory
   USM
      A pointer-based framework for memory management in SYCL_.

   field-programmable gate array
   FPGA
      An integrated circuit designed to be configured *after* manifacturing.
      Check the `wikipedia page
      <https://en.wikipedia.org/wiki/Field-programmable_gate_array>`_.

   RAII
   resource acquisition is initialization
      A programming pattern for safe and automated management of resources,
      *i.e.* memory, files, etc. See `examples on cppreference
      <https://en.cppreference.com/w/cpp/language/raii>`_.

   STL
   standard template library
      Collection of generic data structures and algorithms described in the C++ standard.

   queue
   queues
      SYCL abstraction for scheduling tasks on available devices.

   selector
   selectors
      SYCL type used to map queues to devices.

   vector engines
      A CPU with efficient instruction set for large one-dimensional arrays. See: https://en.wikipedia.org/wiki/Vector_processor
