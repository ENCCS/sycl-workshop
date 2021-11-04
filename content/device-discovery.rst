.. _device-discovery:

Device discovery
================

.. questions::

   - How can we make SYCL_ aware of the available hardware?
   - Is it possible to specialize our code for the available hardware?

.. objectives::

   - Learn how to query device information with ``get_info``.
   - Learn how to use and write :term:`selectors`.


The examples in the :ref:`what-is-sycl` episode highlighted the importance of
the :term:`queue` abstraction in SYCL_. All device code is **submitted** to a
:term:`queue` as *actions*:

.. code:: c++

   queue Q;

   Q.submit(
     /* device code action */
   );

the runtime **schedules** the actions and executes them **asynchronously**.
We will discuss queues in further detail in :ref:`queues-cgs-kernels`. At this
point, it is important to stress that a queue can be mapped to one device only.
The mapping happens at queue construction and cannot be changed afterwards.

We have five strategies to run our device code:

Somewhere
   This is what we have done so far and it's achieved by simply using the
   ``queue`` object default constructor:

   .. code:: c++

      queue Q;

   Most likely you want to have more control on what device queues in your code
   will use, especially as your SYCL code matures.

In order to gain more control, we will use the following constructor:

.. signature:: ``queue`` constructor

   .. code:: c++

      template <typename DeviceSelector>
      explicit queue(const DeviceSelector &deviceSelector,
                     const property_list &propList = {});

The **selector** passed as first parameter lets us specify *how* the runtime
should go about mapping the queue to a device.

On the *host device*
   A standards-compliant SYCL implementation will always define a host device
   and we can bind a queue to it by passing the ``host_selector`` object to its
   constructor:

   .. code:: c++

      queue Q{host_selector{}};

   The host device makes the host CPU "look like" an *independent* device, such
   that device code will run regardless of whether the hardware is available.
   This is especially useful in three scenarios:

   1. Developing heterogeneous code on a machine without hardware.
   2. Debugging device code using CPU tooling.
   3. Fallback option to guarantee functional portability.

On a *specific* class of devices
   Such as GPUs or FPGAs. The SYCL_ standard defines a few *selectors* for this
   use case.

   .. code:: c++

      queue Q_cpu{default_selector{}};

      queue Q_cpu{cpu_selector{}};

      queue Q_device{gpu_selector{}};

      queue Q_accelerator{accelerator_selector{}};


   - ``default_selector`` is the implementation-defined default device. This is
     not portable between SYCL compilers.
   - ``cpu_selector`` a CPU device.
   - ``gpu_selector`` a GPU device.
   - ``accelerator_selector`` an accelerator device, including FPGAs.

On a *specific* device in a *specific* class
   For example on a GPU with well-defined compute capabilities. SYCL_ defines
   the ``device_selector`` base class, which we can inherit from and customize
   to our needs.

   .. code:: c++

      class special_device_selector : public device_selector {
        /* we will look at what goes here soon! */
      };

      queue Q{special_device_selector{}};


   Coincidentally, this is the most flexible and portable way of
   *parameterizing* our code to work on a diverse set of devices.


.. exercise:: hipSYCL and ``HIPSYCL_TARGETS``

   SYCL_ is all about being able to write code *once* and execute it on
   different hardware. The sample code in folder
   ``content/code/day-1/01_hello-selectors`` used the default queue constructor:
   essentially, the runtime decides which device will be used for us.
   Let us explore how that works with hipSYCL_. When
   configuring the code we can set the ``HIPSYCL_TARGETS`` CMake
   option to influence the behavior.

   1. Compile to target the GPU, using ``-DHIPSYCL_TARGETS="cuda:sm_80"`` in the
      configuration step.  What output do you see? Is the code running on the
      device you expect?
   2. Compile to target the GPU *and* the host device using OpenMP with
      ``-DHIPSYCL_TARGETS="cuda:sm_80;omp"``. What output do you see now?
   3. Extend the code to create multiple queues, each using one of the standard
      selectors, and compile with ``-DHIPSYCL_TARGETS="cuda:sm_80;omp"``.
      What output do you expect to see?

   .. note::

      ``accelerator_selector`` is not implemented in hipSYCL_ 0.9.1

   To learn more about the compilation model in hipSYCL_, check out `its
   documentation
   <https://github.com/illuhad/hipSYCL/blob/develop/doc/compilation.md>`_.


Writing your own selector
-------------------------

Using inheritance
~~~~~~~~~~~~~~~~~

All the standard selectors are derived types of the abstract ``device_selector``
class. This class defines, among other things, a pure virtual overload of the
function-call operator:

.. code:: c++

   virtual int operator()(const device &dev) const = 0;

The method takes a ``device`` object and return a **score** for it, an integer
value, and the highest score gets selected.
The runtime will call this method exactly once for each device that it has
access too, in order to build the ranking of device scores.
Devices might be completely excluded from the ranking if their score is a
*negative* number.
We can write our own selector by simply inheriting from this abstract base class
and implementing our own custom logic for scoring devices:

.. literalinclude:: code/snippets/inherit-device_selector.cpp
   :language: c++


.. exercise:: Write a custom selector

   It's not that far of a stretch to imagine that in a not-so-distant future, a
   node in a cluster might be equipped with accelarators from different vendors.
   In this exercise, you'll write a selector to score GPUs from different
   vendors according to your preferences.

   You can find a scaffold for the code in the
   ``content/code/day-1/02_custom-selectors/custom-selectors.cpp`` file,
   alongside the CMake script to build the executable. You will have to complete
   the source code to compile and run correctly: follow the hints in the source
   file.  The solution is in the ``solution`` subfolder.

   #. Load the necessary modules:

      .. code:: console

         $ module load CMake hipSYCL

   #. Configure, compile, and run the code:

      .. code:: console

         $ cmake -S. -Bbuild -DHIPSYCL_TARGETS="omp"
         $ cmake --build build -- VERBOSE=1
         $ ./build/custom-selectors


   Try compiling and executing on a non-GPU node. What happens? How can you make
   the code more robust?


Using aspects
~~~~~~~~~~~~~

The standard defines the ``aspect_selector`` free function, which
return a selectors based on desired device **aspects**:

.. signature:: ``aspect_selector``

   .. code:: c++

      template <class... aspectListTN>
      auto aspect_selector(aspectListTN... aspectList);

Available aspects are defined in the ``aspect`` enumeration and can be probed
using the ``has`` method of the ``device`` class. For example,
``dev.has(aspect::gpu)`` is equivalent to ``dev.is_gpu()``.
A selector for GPUs supporting half-precision floating-point numbers (FP16) and :term:`USM` device allocations can be implemented with a one-liner:

.. code:: c++

   auto my_selector = aspect_selector(aspect::usm_device_allocations, aspect::fp16);

The aspects available, according to the standard, are `available here
<https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#sec:device-aspects>`_.
Currently, we cannot use aspects to filter devices based on vendors.

Introspection with ``get_info``
-------------------------------

It is not a good idea to depend explicitly on vendor and/or device names in our
program: for maximum portability, our device code should rather be parameterized
on *compute capabilities*, *available memory*, and so forth.
Introspection into such parameters is achieved with the ``get_info`` template function:

.. signature:: ``get_info``

   .. code:: c++

      template <typename param>
      typename param::return_type get_info() const;

This is a method available for many of the classes defined by the SYCL standard
including ``device``, of course. The template parameter specifies which
information we would like to obtain.
In the previous examples, we have used ``info::device::vendor`` and
``info::device::name`` to build our selectors.
Valid ``get_info`` queries for devices are in the ```info::device`` namespace
and can be roughly classified in two groups of queries, which can:

#. decide whether a kernel can run correctly on a given device. For example,
   querying for ``info::device::global_mem_size`` and
   ``info::device::local_mem_size`` would return the size, in bytes, of the
   global and local memory, respectively.
#. help tune kernel code to a given device. For example, querying
   ``info::device::local_mem_type`` would return which kind of local memory is
   available on the device: none, dedicated local storage, or an abstraction
   built using global memory.

We will not list all possible device queries here: a complete list is `available
on the webpage of the standard
<https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#_device_information_descriptors>`_.


.. exercise:: Nosing around on our system

   We will write a chatty program to report properties of all devices available
   on our system. We will have to keep the `list of queries
   <https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#_device_information_descriptors>`_
   at hand for this task.

   You can find a scaffold for the code in the
   ``content/code/day-1/03_platform-devices/platform-devices.cpp`` file,
   alongside the CMake script to build the executable. You will have to complete
   the source code to compile and run correctly: follow the hints in the source
   file.  The solution is in the ``solution`` subfolder.

   The code is a double loop:

   #. First over all *platforms*

      .. code:: c++

         for (const auto & p : platform::get_platforms()) {
           ...
         }

      A ``platform`` is an abstraction mapping to a backend.

   #. Then over all *devices* available on each platform:

      .. code:: c++

         for (const auto& d: p.get_devices()) {
           ...
         }

   #. We will query the device with ``get_info`` in the inner loop. For example:

      .. code:: c++

         std::cout << "name: " << d.get_info<info::device::name>() << std::endl;

   #. Add queries and print out information on global and local memory, work
      items, and work groups.


The ``info::`` namespace is **vast!** You can query many aspects of a SYCL code
at runtime using ``get_info``, not just devices. The classes ``platform``,
``context``, ``queue``, ``event``, and ``kernel`` also offer a ``get_info``
method. The queries in the ``info::kernel_device_specific``  `namespace
<https://www.khronos.org/registry/SYCL/specs/sycl-2020/html/sycl-2020.html#table.kernel.devicespecificinfo>`_
can be helpful with performance tuning.

.. keypoints::

   - Device selection is essential to tailor execution to the available hardware
     and is achieved using the ``device_selector`` abstraction.
   - Custom selectors with complex logic can be implemented with inheritance.
   - You should use ``get_info`` to probe your system. Device selection should
     be done based on compute capabilities, not on vendor and/or device names.
