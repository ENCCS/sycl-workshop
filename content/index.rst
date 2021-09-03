SYCL
====


The high-performance computing landscape is increasingly dominated by machines whose high FLOP counts is delivered by _heterogeneous hardware_: large-core count CPUs in tandem with ever more powerful GPUs are now the norm in the HPC datacenter.
This trend is likely to continue, with the appearance of new hardware architectures, sometimes tailored for specific operations.

Each new architecture comes equipped with its own, usually low-level, programming language. Adapting applications for a heterogeneous computing environment proceeds in tight cycles of profiling and porting. These are time-consuming, error-prone, and scarcely portable:

* Mastering low-level programming languages may require years.
* The codebase can _diverge_ significantly to work with different hardware.

`SYCL <https://www.khronos.org/sycl/>`_ is a C++ abstraction layer for programming heterogeneous hardware in a **high-level**, **cross-platform** fashion using **standard** ISO C++. Furthermore, SYCL is a **single-source** approach: you can exppress host and device operations within the same source file.


.. prereq::

   prerequisites



.. csv-table::
   :widths: auto
   :delim: ;

   20 min ; :doc:`what-is-sycl`
   20 min ; :doc:`queues-cgs-kernels`
   20 min ; :doc:`buffers-accessors`
   20 min ; :doc:`unified-shared-memory`


.. toctree::
   :hidden
   :maxdepth: 1
   :caption: The lesson

   what-is-sycl
   queues-cgs-kernels
   buffers-accessors
   unified-shared-memory


.. toctree::
   :maxdepth: 1
   :caption: Reference

   quick-reference
   zbibliography
   guide



.. _learner-personas:

Who is the course for?
----------------------

- A working knowledge of recent C++ standards.
- Some familiarity with C++17. In particular, class template argument deduction (CTAD).



About the course
----------------



Graphical and text conventions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We adopt a few conventions which help organize the material.

Function signatures
   These are shown in a text block marked with a wrench emoji:

   .. signature:: |cmake_minimum_required|

      .. code-block:: cmake

         cmake_minimum_required(VERSION <min>[...<max>] [FATAL_ERROR])

   The signature can be hidden by clicking the toggle.

Command parameters
   The description of the command parameters will appear in a separate text
   box. It will be marked with a laptop emoji:

   .. parameters::

      ``VERSION``
          Minimum and, optionally, maximum version of CMake to use.
      ``FATAL_ERROR``
          Raise a fatal error if the version constraint is not satisfied. This
          option is ignored by CMake >=2.6

   The description is hidden and will be shown by clicking the toggle.

Type-along
   The text and code for these activities are in a separate text box, marked with
   a keyboard emoji:

   .. typealong:: Let's look at an example

      .. code-block:: cmake

         cmake_minimum_required(VERSION 3.16)

         project(Hello LANGUAGES CXX)

   The content can be hidden by clicking the toggle.






See also
--------

- Reinders, J.; Ashbaugh, B.; Brodman, J.; Kinsner, M.; Pennycook, J.; Tian, X.
  *Data Parallel C++: Mastering DPC++ for Programming of Heterogeneous Systems
  Using C++ and SYCL* :cite:`Reinders2021-yx`
- The `SYCL Academy <https://github.com/codeplaysoftware/syclacademy>`_ resources.


Credits
-------

The lesson file structure and browsing layout is inspired by and derived from
`work <https://github.com/coderefinery/sphinx-lesson>`_ by `CodeRefinery
<https://coderefinery.org/>`_ licensed under the `MIT license
<http://opensource.org/licenses/mit-license.html>`_. We have copied and adapted
most of their license text.

Instructional Material
^^^^^^^^^^^^^^^^^^^^^^

This instructional material is made available under the
`Creative Commons Attribution license (CC-BY-4.0) <https://creativecommons.org/licenses/by/4.0/>`_.
The following is a human-readable summary of (and not a substitute for) the
`full legal text of the CC-BY-4.0 license
<https://creativecommons.org/licenses/by/4.0/legalcode>`_.
You are free to:

- **share** - copy and redistribute the material in any medium or format
- **adapt** - remix, transform, and build upon the material for any purpose,
  even commercially.

The licensor cannot revoke these freedoms as long as you follow these license terms:

- **Attribution** - You must give appropriate credit (mentioning that your work
  is derived from work that is Copyright (c) Roberto Di Remigio and individual contributors and, where practical, linking
  to `<https://enccs.se>`_), provide a `link to the license
  <https://creativecommons.org/licenses/by/4.0/>`_, and indicate if changes were
  made. You may do so in any reasonable manner, but not in any way that suggests
  the licensor endorses you or your use.
- **No additional restrictions** - You may not apply legal terms or
  technological measures that legally restrict others from doing anything the
  license permits.

With the understanding that:

- You do not have to comply with the license for elements of the material in
  the public domain or where your use is permitted by an applicable exception
  or limitation.
- No warranties are given. The license may not give you all of the permissions
  necessary for your intended use. For example, other rights such as
  publicity, privacy, or moral rights may limit how you use the material.



Software
^^^^^^^^

Except where otherwise noted, the example programs and other software provided
with this repository are made available under the `OSI <http://opensource.org/>`_-approved
`MIT license <https://opensource.org/licenses/mit-license.html>`_.

