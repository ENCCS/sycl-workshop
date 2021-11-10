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

// Utility functions for heat equation solver

#include <cassert>
#include <cstdlib>

#include "heat.h"

// Copy data on temperature1 into temperature2
void
copy_field(field *temperature1, field *temperature2)
{
  assert(temperature1->nx == temperature2->nx);
  assert(temperature1->ny == temperature2->ny);
  assert(temperature1->data.size() == temperature2->data.size());
  std::copy(
    temperature1->data.begin(),
    temperature1->data.end(),
    temperature2->data.begin());
}

// Swap the field data for temperature1 and temperature2
void
swap_fields(field *temperature1, field *temperature2)
{
  std::swap(temperature1->data, temperature2->data);
}

// Allocate memory for a temperature field and initialise it to zero
void
allocate_field(field *temperature)
{
  // Include also boundary layers
  int newSize = (temperature->nx + 2) * (temperature->ny + 2);
  temperature->data.resize(newSize, 0.0);
}

// Calculate average temperature over the non-boundary grid cells
double
average(field *temperature)
{
  double average = 0.0;

  for (int i = 1; i < temperature->nx + 1; i++) {
    for (int j = 1; j < temperature->ny + 1; j++) {
      int ind = i * (temperature->ny + 2) + j;
      average += temperature->data[ind];
    }
  }

  average /= (temperature->nx * temperature->ny);
  return average;
}
