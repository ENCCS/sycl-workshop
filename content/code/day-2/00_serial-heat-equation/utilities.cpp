// Utility functions for heat equation solver

#include <cstdlib>
#include <cassert>

#include "heat.h"


// Copy data on temperature1 into temperature2
void copy_field(field *temperature1, field *temperature2)
{
    assert(temperature1->nx == temperature2->nx);
    assert(temperature1->ny == temperature2->ny);
    assert(temperature1->data.size() == temperature2->data.size());
    std::copy(temperature1->data.begin(), temperature1->data.end(),
              temperature2->data.begin());
}

// Swap the field data for temperature1 and temperature2
void swap_fields(field *temperature1, field *temperature2)
{
    std::swap(temperature1->data, temperature2->data);
}

// Allocate memory for a temperature field and initialise it to zero
void allocate_field(field *temperature)
{
    // Include also boundary layers
    int newSize = (temperature->nx + 2) * (temperature->ny + 2);
    temperature->data.resize(newSize, 0.0);
}

// Calculate average temperature over the non-boundary grid cells
double average(field *temperature)
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


