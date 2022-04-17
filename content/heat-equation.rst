.. _heat-equation:

Heat equation mini-app
======================

.. objectives::

   - Understand the structure of a mini-app that models heat diffusion
   - Understand how the 5-point stencil operates
   - Understand that the loops influence the duration of the mini-app
   - Understand the expected output of the mini-app

.. note::

   This episode was adapted, with permission, from the `ENCCS/CSC workshop on
   OpenMP for GPU offloading <https://enccs.github.io/openmp-gpu/>`_.  CSC
   staff developed the serial version of the mini-app, under MIT license. We
   reproduce it with permission.


Heat equation
-------------

Heat flows in objects according to local temperature differences, as if seeking
local equilibrium.
Such processes can be modelled with partial differential equations via
discretization to a regular grid.
Solving for the flow over time can involve a lot of computational effort.
Fortunately that effort is quite regular and so can suit parallelization with a
variety of techniques, SYCL_ included.

The partial differential equation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The rate of change of the temperature field :math:`u(x, y, t)` over two spatial
dimensions :math:`x` and :math:`y` and time :math:`t`
with diffusivity :math:`\alpha` can be modelled via the equation

.. math::
   \frac{\partial u}{\partial t} = \alpha \nabla^2 u

where :math:`\nabla` is the Laplacian operator, which describes how
the temperature field varies with the spatial dimensions :math:`x` and
:math:`y`. When those are continuous variables, that looks like

.. math::
   \frac{\partial u}{\partial t} = \alpha \left( \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial x^2}\right)

Because computers are finite devices, we often need to solve such equations
numerically, rather than analytically.
This often involves *discretization*, where spatial and temporal variables only
take on specific values from a set.
In this mini-app we will discretize all three dimensions :math:`x`, :math:`y`,
and :math:`t`, such that

.. math::
   \nabla^2 u  &= \frac{u(i-1,j)-2u(i,j)+u(i+1,j)}{(\Delta x)^2} \\
       &+ \frac{u(i,j-1)-2u(i,j)+u(i,j+1)}{(\Delta y)^2}

where :math:`u(i,j)` refers to the temperature at location with
integer index :math:`i` within the domain of :math:`x` spaced by
:math:`\Delta x` and location with integer index :math:`j` within the
domain of :math:`y` spaced by :math:`\Delta y`.

Given an initial condition :math:`(u^{t=0})`, one can follow the time
dependence of the temperature field from state :math:`m` to
:math:`m+1` over regular time steps :math:`\Delta t` with explicit
time evolution method:

.. math::
    u^{m+1}(i,j) = u^m(i,j) + \Delta t \alpha \nabla^2 u^m(i,j)

This equation expresses that the time evolution of the temperature
field at a particular location depends on the value of the field at
the previous step at the same location *and* four adjacent locations:

.. figure:: img/stencil.svg
   :align: center

   This example model uses an 8x8 grid of data in light blue in state
   :math:`m`, each location of which has to be updated based on the
   indicated 5-point stencil in yellow to move to the next time point
   :math:`m+1`.

.. exercise::

   How much arithmetic must be done to evolve each location at each time step?

.. solution::

   10 arithmetic operations per location per time step. 3 in each of 2
   numerators, 1 to divide by each pre-computed denominator, and two
   additions to update :math:`u`.

.. exercise::

   How much arithmetic must be done to evolve all locations in the grid for 20 steps?

.. solution::

   There's 64 locations and 20 steps and each does the same 10
   operations, so :math:`10*8*8*20 = 12800` arithmetic operations
   total.

Spatial boundary conditions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Something must happen at the edges of the grid so that the stencil does a valid
operation.  One alternative is to ignore the contribution of points that are
outside the grid.  However, this tends to complicate the implementation of the
stencil and is also often non-physical.  In a real problem, there is always
somethign outside the grid!  Sometimes it makes sense to have periodic
boundaries to the grid, but that is complex to implement.  In this mini-app, we
will have a ring of data points around the grid.  Those will have a fixed value
that is not updated by the stencil, although they do contribute to the stencil
operation for their neighbors.

.. figure:: img/stencil-fixed-boundaries.svg
   :align: center

   This example model uses an 8x8 grid of data in light blue with an
   outer ring in red of boundary grid sites whose temperature values
   are fixed. This lets the stencil operate on the blue region in a
   straightforward way.

The source code
~~~~~~~~~~~~~~~

Now we'll take a look at the source code that will do this for us!
Let's look at the data structure describing the field:

.. typealong:: The field data structure

   .. literalinclude:: code/day-2/06_serial-heat-equation/heat.h
      :language: cpp
      :lines: 27-38


Next, the routine that applies the stencil to the previous field to compute the current one:

.. typealong:: The core evolution operation

   .. literalinclude:: code/day-2/06_serial-heat-equation/core.cpp
      :language: cpp
      :lines: 32-60


Then the routine that handles the main loop over time steps:

.. typealong:: The main driver function

   .. literalinclude:: code/day-2/06_serial-heat-equation/main.cpp
      :language: cpp
      :lines: 35-39,47-54,58-67


There's other supporting code to handle user input and produce nice images of
the current field, but we won't need to touch those, so we won't spend time
looking at them now.
In the real version of the code we have seen, there's also calls to libraries to
record the time taken.  We'll need that later so we understand how fast our code
is.

We should look at the routines that initialize the field data structures:

.. typealong:: The setup routines

   .. literalinclude:: code/day-2/06_serial-heat-equation/utilities.cpp
      :language: cpp
      :lines: 49-56


Building the code
~~~~~~~~~~~~~~~~~

The code is set up so that you can change to its directory
and build as follows.

.. typealong:: Building the code

   .. code-block:: bash

      $ cmake -S. -Bbuild
      $ cmake --build build

which produces an executable program called ``heat`` in the ``build`` folder.
The app can be built with visualization support. [*]_


Running the code
~~~~~~~~~~~~~~~~

The code lets you choose the spatial dimensions and the number of time steps on the command line.
For example, to run an 800 by 800 grid for 1000 steps, run

.. code-block:: bash

   ./heat 800 800 1000

Try it now!

.. exercise::

   How long does the iteration take if you double the number of steps?
   How long does the iteration take if you double the number of grid points in each direction?

.. solution::

   Doubling the number of steps doubles the total amount of work, so should take around twice as long.
   Doubling both numbers of grid points is four times as much work, so should take around four times as long.

You can see the output on the terminal, like::

  Average temperature at start: 59.762281
  Iterations took 0.426 seconds.
  Average temperature: 58.065097

This report will help us check whether our attempts to optimize made the code faster while keepint it correct.

Initial and boundary conditions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When solving PDEs, the initial conditions determine the possible solutions.
The mini-app automatically sets up a disk of cold grid points in the center at
temperature 5, with warm grid points around it at temperature 65.

.. figure:: img/heat_0000.png
   :align: center
   :scale: 50%

   Initial conditions of the grid.
   The boundary layers are not shown.

There is a fixed boundary layer of one grid point on all sides, two of which are
warm (temperature 70 and 85) and two cold (temperature 20 and 5).
Early on, the disk and its surroundings dominate the contents of the grid, but
over time, the boundary layers have greater and greater influence.

.. exercise::

   To which average temperature will the grid converge?

.. solution::

   Eventually, the boundary conditions will dominate.
   Each contributes equally if the sides are of equal length.  The average of
   the grid will be the average of the boundaries, ie. :math:`(70+20+85+5)/4`
   which is :math:`45`.


Writing a SYCL port
-------------------

We are now ready to write a SYCL port of the heat equation mini-app. We will start by using the buffer and accessor model and will look into using USM at a later stage, see :ref:`buffer-accessor-vs-usm`.

Before starting to work with the code, consider:

#. Which kernel should be ported first?
   The time evolution is essentially serial:

   .. literalinclude:: code/day-2/06_serial-heat-equation/main.cpp
      :language: cpp
      :lines: 58-67

   The stencil application will be our target for parallelization with SYCL:

   .. literalinclude:: code/day-2/06_serial-heat-equation/core.cpp
      :language: cpp
      :lines: 32-60

#. How do we fit the queue, buffer, and accessor concepts of SYCL in the
   existing codebase?
   We will have to modify a few aspects of the codebase, to make sure that
   functions to be offloaded are aware of the queue. Data will have to be
   wrapped into SYCL buffers.

.. exercise:: SYCL heat equation mini app with buffers and accessors

   We will use the serial version of the mini-app as a scaffold for our port.
   The code is in the ``content/code/day-2/06_serial-heat-equation`` folder.
   You will have to *uncomment* some lines in the CMake script in order to build
   the executable with SYCL support.
   A working solution is in the ``content/code/day-2/07_sycl-heat-equation``
   folder.

   Let's start from the top, ``main.cpp``, and more down to the stencil
   application function in ``core.cpp``. Compile after each step and fix
   compiler errors before moving on to the next step.

   #. As usual, we first create a queue and map it to the GPU, either explicitly:

      .. code:: c++

         queue Q{gpu_selector{}};

      or implicitly, by compiling with the appropriate ``HIPSYCL_TARGETS`` value.

   #. The queue needs to be passed into the ``evolve`` function.  Compiling now
      should raise an error, because no overload of this function accepting a
      ``queue`` is known. Fix the compiler errors by redefining the function or
      providing an overload.  Don't change the implementation in ``core.cpp``
      yet.

   We can now work on the parallel implementation:

   #. Obtain grid sizes in the :math:`x` and :math:`y` directions from the
      ``curr`` input parameter.
   #. Obtain denominator of the finite-difference formula in both Cartesian
      directions:

      .. code:: c++

         auto dx2 = prev->dx * prev->dx;
         auto dy2 = prev->dy * prev->dy;

   #. We open a new scope for our SYCL work and declare buffers mapping to the
      data underlying the ``curr`` and ``prev`` data structures for the heat
      field:

      .. code:: c++

         buffer<double, 2> buf_curr{ ..., ... }, buf_prev{ ..., ... };

      What are the dimensions of the iteration spaces given as second argument
      to the buffer constructors?  Remember that the edges of the grid
      accomodate the fixed boundary conditions!
   #. With buffers at hand, we're ready to submit work to the queue:

      .. code:: c++

         Q.submit([&](handler& cgh){
            /* body */
         });

   #. First, declare accessors with appropriate targets, since ``curr`` is
      read-write, but ``prev`` is read-only:

      .. code:: c++

         auto acc_curr = accessor(buf_curr, cgh, ...);
         auto acc_prev = accessor(buf_prev, cgh, ...);

   #. To start with, we use a basic data-parallel kernel. Add a ``parallel_for``
      to the command-group handler:

      .. code:: c++

         cgh.parallel_for(range<2>(..., ...), [=](id<2> id) {
           /* kernel body */
         }

      What are the extents of the ``range`` object passed as first argument?
      Remember that the zeroth and last elements in each dimension accommodate
      the values **fixed** by the boundary conditions!
   #. The buffers and accessors reinterpret the data in the 1-dimensional
      ``data`` arrays of the ``field`` data structures as 2-dimensional objects.
      We need to obtain the correct row and column indices:

      .. code:: c++

         auto j = ...;
         auto i = ...;

      Once again, remember that the zeroth and last elements in each dimension
      accommodate the values **fixed** by the boundary conditions!
   #. Finally, apply the stencil:

      .. code:: c++

         acc_curr[j][i] =
           acc_prev[j][i] +
           a * dt *
             ((acc_prev[j][i + 1] - 2.0 * acc_prev[j][i] + acc_prev[j][i - 1]) / dx2 +
              (acc_prev[j + 1][i] - 2.0 * acc_prev[j][i] + acc_prev[j - 1][i]) / dy2);

   The star and stop timers surrounding the time evolution loop in ``main.cpp``
   can be used to give a rough estimate of the performance. Compare the serial
   and SYCL versions of the code? What do you notice?
   At a glance, do you think this is the best SYCL version of the app we can
   write?



.. keypoints::

   - The heat equation is discretized in space and time
   - The implementation has loops over time and spatial dimensions
   - The implementation reports on the contents of the grid so we can understand
     correctness and performance easily.

.. rubric:: Footnotes

.. [*] The mini-app has support for writing an image file that shows the state of the
       grid every 1500 steps.  Below we can see the progression over larger numbers of
       steps:

       ..
          This image was made with the montage tool from ImageMagick.
          Run ./heat_serial 800 800 42000 then
          montage heat_?000.png heat_??000.png heat_montage.png

       .. figure:: img/heat_montage.png
          :align: center

          Over time, the grid progresses from the initial state toward
          an end state where one triangle is cold and one is warm.
          The average temperature tends to 45.

       We can use this visualization to check that our attempts at parallelization are
       working correctly.  Perhaps some bugs can be resolved by seeing what distortions
       they introduce.

       .. note::

          The PNG library is available as a module on the Karolina supercomputer:

          .. code:: console

             $ module load libpng
