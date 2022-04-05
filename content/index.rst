SYCL
====


SYCL_ is a C++ abstraction layer for programming heterogeneous hardware with a
**single-source approach**. SYCL_ is built on top of **standard** ISO C++17 and
provides a **high-level** and **cross-platform** route for heterogeneous
programming.

In this workshop, you will learn to:

* Use the hipSYCL_ compiler to generate executable for *multiple* hardware
  targets.
* Write hardware-agnostic code to express parallelism using the queue, command
  group, and kernel abstractions.
* Use buffer and accessors to handle memory across devices.
* Evaluate drawbacks and advantages of unified shared memory.


.. prereq::

   Before attending this workshop, please make sure that you have access to a
   machine with the hipSYCL compiler (v0.9.1) and CMake (>=3.14) installed.

   This workshop is organized in collaboration with `CSC`_ and `IZUM`_. We will
   work on the exercises using the `Vega supercomputer
   <https://doc.vega.izum.si/>`_, a `EuroHPC Joint Undertaking
   <https://eurohpc-ju.europa.eu/>`_ petascale system.


.. toctree::
   :hidden:
   :maxdepth: 1

   setup


.. toctree::
   :hidden:
   :maxdepth: 1
   :caption: The lesson

   what-is-sycl
   device-discovery
   queues-cgs-kernels
   buffers-accessors
   unified-shared-memory
   expressing-parallelism-basic
   expressing-parallelism-nd-range
   task-graphs-synchronization
   heat-equation
   sub-groups
   profiling
   buffer-accessor-vs-usm


.. toctree::
   :hidden:
   :maxdepth: 1
   :caption: Reference

   quick-reference
   zbibliography
   guide


.. see also the schedule in guide.rst

.. csv-table::
   :widths: auto
   :delim: ;

   30 min ; :doc:`what-is-sycl`
   30 min ; :doc:`device-discovery`
   30 min ; :doc:`queues-cgs-kernels`
   30 min ; :doc:`buffers-accessors`
   30 min ; :doc:`unified-shared-memory`
   30 min ; :doc:`expressing-parallelism-basic`
   30 min ; :doc:`expressing-parallelism-nd-range`
   40 min ; :doc:`task-graphs-synchronization`
   40 min ; :doc:`heat-equation`
   30 min ; :doc:`sub-groups`
   30 min ; :doc:`profiling`
   40 min ; :doc:`buffer-accessor-vs-usm`



.. _learner-personas:

Who is the course for?
----------------------

This course is for students, researchers, engineers, and programmers that have
heard of `SYCL`_ and want to learn how to use it effectively with projects they
are working on.
This course assumes no previous experience with `SYCL`_, however, participants
should have some prior experience with or knowledge of the following:

- A working knowledge of recent C++ standards, *e.g.* familiarity with `lambda
  functions <https://en.cppreference.com/w/cpp/language/lambda>`_, `function
  objects <https://en.cppreference.com/w/cpp/utility/functional>`_, and
  `templates <https://en.cppreference.com/w/cpp/language/templates>`_.
- Some familiarity with C++17, *e.g.* `class template argument deduction (CTAD)
  <https://en.cppreference.com/w/cpp/language/class_template_argument_deduction>`_
  and `structured binding declaration
  <https://en.cppreference.com/w/cpp/language/structured_binding>`_.


About the course
----------------

This lesson material is developed by the `EuroCC National Competence Center
Sweden (ENCCS) <https://enccs.se/>`_, CSC_, and IZUM_.
It is taught in ENCCS workshops.
It is aimed at researchers and developers who are curious about `SYCL`_ and how
it can help them leverage heterogeneous hardware effectively.
This lesson targets **SYCL 2020**.
Each lesson episode has clearly defined learning objectives and includes
multiple exercises along with solutions, and is therefore also useful for
self-learning.
The lesson material is licensed under `CC-BY-4.0
<https://creativecommons.org/licenses/by/4.0/>`_ and can be reused in any form
(with appropriate credit) in other courses and workshops.
Instructors who wish to teach this lesson can refer to the :doc:`guide` for
practical advice.


Graphical and text conventions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We adopt a few conventions which help organize the material.

Function signatures
   These are shown in a text block marked with a wrench emoji:

   .. signature:: ``struct plus``

      .. code-block:: c++

         template <typename T=void>
         struct plus {
           T operator()(const T& x, const T& y) const;
         };

   The signature can be hidden by clicking the toggle.

Function arguments
   The description of the function arguments will appear in a separate text
   box. It will be marked with a laptop emoji:

   .. parameters::

      ``T``
          Scalar type.
      ``x``
          First operand.
      ``y``
          Second operand.

   The description is hidden and will be shown by clicking the toggle.

Type-along
   The text and code for these activities are in a separate text box, marked with
   a keyboard emoji:

   .. typealong:: Let's look at an example

      .. code-block:: c++

         cgh.parallel_for(1024, [=](auto idx) {
            x[idx] = idx;
         });

   The content can be hidden by clicking the toggle.






See also
--------

- Reinders, J.; Ashbaugh, B.; Brodman, J.; Kinsner, M.; Pennycook, J.; Tian, X.
  *Data Parallel C++: Mastering DPC++ for Programming of Heterogeneous Systems
  Using C++ and SYCL* :cite:`Reinders2021-yx`
- The `SYCL Academy <https://github.com/codeplaysoftware/syclacademy>`_ resources.
- The `hipSYCL blog <https://hipsycl.github.io/>`_ and `GitHub repository
  <https://github.com/illuhad/hipSYCL>`_.
- The latest revision of the `SYCL 2020 standard
  <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html>`_.
- The `SYCL 2020 API reference guide
  <https://www.khronos.org/files/sycl/sycl-2020-reference-guide.pdf>`_.


Credits
-------

The lesson file structure and browsing layout is inspired by and derived from
`work <https://github.com/coderefinery/sphinx-lesson>`_ by `CodeRefinery
<https://coderefinery.org/>`_ licensed under the `MIT
<http://opensource.org/licenses/mit-license.html>`_ license. We have copied and adapted
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
MIT_ license.
