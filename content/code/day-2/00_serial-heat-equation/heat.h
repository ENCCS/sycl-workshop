#pragma once

#include <vector>

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
evolve(field *curr, field *prev, double a, double dt);

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
