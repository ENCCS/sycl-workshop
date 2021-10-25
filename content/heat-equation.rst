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
   OpenMP for GPU offloading <https://enccs.github.io/openmp-gpu/>`_.


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
---------------------------------

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
---------------------------

Something must happen at the edges of the grid so that the stencil does a valid operation.
One alternative is to ignore the contribution of points that are outside the grid.
However, this tends to complicate the implementation of the stencil and is also often non-physical.
In a real problem, there is always somethign outside the grid!
Sometimes it makes sense to have periodic boundaries to the grid, but that is complex to implement.
In this mini-app, we will have a ring of data points around the grid.
Those will have a fixed value that is not updated by the stencil,
although they do contribute to the stencil operation for their neighbors.

.. figure:: img/stencil-fixed-boundaries.svg
   :align: center

   This example model uses an 8x8 grid of data in light blue with an
   outer ring in red of boundary grid sites whose temperature values
   are fixed. This lets the stencil operate on the blue region in a
   straightforward way.

The source code
---------------

Now we'll take a look at the source code that will do this for us!
Let's look at the data structure describing the field:

.. typealong:: The field data structure

   .. literalinclude:: code/day-2/00_serial-heat-equation/heat.h
      :language: cpp
      :lines: 6-17


Next, the routine that applies the stencil to the previous field to compute the current one:

.. typealong:: The core evolution operation

   .. literalinclude:: code/day-2/00_serial-heat-equation/core.cpp
      :language: cpp
      :lines: 11-39


Then the routine that handles the main loop over time steps:

.. typealong:: The main driver function

   .. literalinclude:: code/day-2/00_serial-heat-equation/main.cpp
      :language: cpp
      :lines: 8-10,14-18,26-33,37-46,64-65


There's other supporting code to handle user input and produce nice images
of the current field, but we won't need to touch those, so we won't spend time
looking at them now.
In the real version of the code we have seen, there's also calls to libraries to record the time taken.
We'll need that later so we understand how fast our code is.

We should look at the routines that initialize the field data structures:

.. typealong:: The setup routines

   .. literalinclude:: code/day-2/00_serial-heat-equation/utilities.cpp
      :language: cpp
      :lines: 28-35


Building the code
-----------------

The code is set up so that you can change to its directory,
type `make` and it will build and run for you.

.. typealong:: Building the code

   .. code-block:: bash

      $ cmake -S. -Bbuild
      $ cmake --build build

which produces an executable program called ``heat`` in the ``build`` folder.

Running the code
----------------

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
-------------------------------

When solving PDEs, the initial conditions determine the possible solutions.
The mini-app automatically sets up a disk of cold grid points in the center at temperature 5, with warm grid points around it at temperature 65.

.. figure:: img/heat_0000.png
   :align: center
   :scale: 50%

   Initial conditions of the grid.
   The boundary layers are not shown.

There is a fixed boundary layer of one grid point on all sides, two of which are warm (temperature 70 and 85) and two cold (temperature 20 and 5).
Early on, the disk and its surroundings dominate the contents of the grid, but over time, the boundary layers have greater and greater influence.

.. exercise::

   To which average temperature will the grid converge?

.. solution::

   Eventually, the boundary conditions will dominate.
   Each contributes equally if the sides are of equal length.
   The average of the grid will be the average of the boundaries, ie. :math:`(70+20+85+5)/4` which is :math:`45`.


Visualizing the output
----------------------

The mini-app has support for writing an image file that shows the state of the grid every 1500 steps.
Below we can see the progression over larger numbers of steps:

..
   This image was made with the montage tool from ImageMagick.
   Run ./heat_serial 800 800 42000 then
   montage heat_?000.png heat_??000.png heat_montage.png

.. figure:: img/heat_montage.png
   :align: center

   Over time, the grid progresses from the initial state toward
   an end state where one triangle is cold and one is warm.
   The average temperature tends to 45.

We can use this visualization to check that our attempts at parallelization are working correctly.
Perhaps some bugs can be resolved by seeing what distortions they introduce.


.. keypoints::

   - The heat equation is discretized in space and time
   - The implementation has loops over time and spatial dimensions
   - The implementation reports on the contents of the grid so we can understand correctness and performance easily.
