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

// Main solver routines for heat equation solver

#include "heat.h"

#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

// Update the temperature values using five-point stencil
// Arguments:
//   curr: current temperature values
//   prev: temperature values from previous time step
//   a: diffusivity
//   dt: time step
void
evolve(queue Q, field *curr, field *prev, double a, double dt)
{
  // Help the compiler avoid being confused by the structs
  auto nx = curr->nx;
  auto ny = curr->ny;

  // Determine the temperature field at next time step
  // As we have fixed boundary conditions, the outermost gridpoints
  // are not updated.
  auto dx2 = prev->dx * prev->dx;
  auto dy2 = prev->dy * prev->dy;

  {
    buffer<double, 2> buf_curr { curr->data.data(), range<2>(nx + 2, ny + 2) },
      buf_prev { prev->data.data(), range<2>(nx + 2, ny + 2) };

    Q.submit([&](handler &cgh) {
      auto curr = accessor(buf_curr, cgh, read_write);
      auto prev = accessor(buf_prev, cgh, read_only);

      cgh.parallel_for(range<2>(nx, ny), [=](id<2> id) {
        auto j = id[0] + 1;
        auto i = id[1] + 1;

        curr[j][i] =
          prev[j][i] +
          a * dt *
            ((prev[j][i + 1] - 2.0 * prev[j][i] + prev[j][i - 1]) / dx2 +
             (prev[j + 1][i] - 2.0 * prev[j][i] + prev[j - 1][i]) / dy2);
      });
    });
  }
}
