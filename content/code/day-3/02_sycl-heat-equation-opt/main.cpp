/* Copyright (c) 2021 CSC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// Main routine for heat equation solver in 2D.

#include <chrono>
#include <cstdio>

#include <sycl/sycl.hpp>

#include "heat.h"

using namespace sycl;

int
main(int argc, char **argv)
{
  // Image output interval
  int image_interval = 1500;

  // Number of time steps
  int nsteps;
  // Current and previous temperature fields
  field current, previous;
  initialize(argc, argv, &current, &previous, &nsteps);

  // Output the initial field
  write_field(&current, 0);

  double average_temp = average(&current);
  printf("Average temperature at start: %f\n", average_temp);

  // Diffusion constant
  double a = 0.5;

  // Compute the largest stable time step
  auto dx = current.dx;
auto dy = current.dy;
  double dx2 = dx * dx;
  double dy2 = dy * dy;
  // Time step
  double dt = dx2 * dy2 / (2.0 * a * (dx2 + dy2));

auto nx = static_cast<size_t>(current.nx);
auto ny = static_cast<size_t>(current.ny);

  using wall_clock_t = std::chrono::high_resolution_clock;

  // create a queue
  queue Q;

  // create buffers for current and previous fields
  buffer<double, 2> buf_curr{current.data.data(), range<2>{nx+2, ny+2}}, 
  buf_prev{previous.data.data(), range<2>{nx+2, ny+2}}; 

  auto start = wall_clock_t::now();

  // Time evolution
  for (int iter = 1; iter <= nsteps; iter++) {
    evolve(Q, buf_curr, buf_prev, a, dt, dx, dy);
    //evolve(Q, &current, &previous, a, dt);
    
    //if (iter % image_interval == 0) {
    //  write_field(&current, iter);
    //}
   
    // Swap current field so that it will be used
    // as previous for next iteration step
    swap_fields(buf_curr, buf_prev);
    //swap_fields(&current, &previous);
  }
Q.wait();

  auto stop = wall_clock_t::now();

  // Average temperature for reference
  average_temp = average(&previous);

  // Determine the CPU time used for all the iterations
  std::chrono::duration<float> elapsed = stop - start;
  printf("Iterations took %.3f seconds.\n", elapsed.count());
  printf("Average temperature: %f\n", average_temp);
  if (argc == 1) {
    printf("Reference value with default arguments: 59.281239\n");
  }

  // Output the final field
  write_field(&previous, nsteps);

  return 0;
}
