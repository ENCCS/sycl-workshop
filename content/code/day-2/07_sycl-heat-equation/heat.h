/* Copyright (c) 2021 CSC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <vector>

#include <sycl/sycl.hpp>

// Datatype for temperature field
struct field
{
  // nx and ny are the dimensions of the field. The array data
  // contains also ghost layers, so it will have dimensions nx+2 x ny+2
  int nx;
  int ny;
  // Size of the grid cells
  double dx;
  double dy;
  // The temperature values in the 2D grid
  std::vector<double> data;
};

// We use here fixed grid spacing
constexpr auto DX = 0.01;
constexpr auto DY = 0.01;

// Function prototypes
void
set_field_dimensions(field *temperature, int nx, int ny);

void
initialize(
  int argc,
  char *argv[],
  field *temperature1,
  field *temperature2,
  int *nsteps);

void
generate_field(field *temperature);

double
average(field *temperature);

void
evolve(sycl::queue Q, field *curr, field *prev, double a, double dt);

void
write_field(field *temperature, int iter);

void
read_field(field *temperature1, field *temperature2, char *filename);

void
copy_field(field *temperature1, field *temperature2);

void
swap_fields(field *temperature1, field *temperature2);

void
allocate_field(field *temperature);
