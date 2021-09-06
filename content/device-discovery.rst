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


.. challenge:: Finding your devices

   Download the :download:`scaffold project <code/tarballs/01_libraries-cxx.tar.bz2>`.

   #. Write a ``CMakeLists.txt`` to compile the source files
      ``Message.hpp`` and  ``Message.cpp`` into a library. Do not specify
      the type of library, shared or static, explicitly.
   #. Add an executable from the ``hello-world.cpp`` source file.
   #. Link the library into the executable.

   You can download the :download:`complete, working example <code/tarballs/01_libraries-cxx_solution.tar.bz2>`.



.. keypoints::

   .. todo::

      Write me
