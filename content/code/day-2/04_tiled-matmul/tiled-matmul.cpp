/* Adapted from: https://github.com/Apress/data-parallel-CPP
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <iostream>
#include <random>

#include <sycl/sycl.hpp>

using namespace sycl;

std::string
local_memory_type(const device& dev)
{

  switch (dev.get_info<info::device::local_mem_type>()) {
    case info::local_mem_type::local:
      return "local";
    case info::local_mem_type::global:
      return "global";
    case info::local_mem_type::none:
      return "none";
    default:
      return "none";
  }
}

int
main()
{
  // set up queue on any available device
  queue Q;

  std::cout << " Device " << Q.get_device().get_info<info::device::vendor>()
            << " " << Q.get_device().get_info<info::device::name>()
            << "\n   Local memory type " << local_memory_type(Q.get_device())
            << "\n   Local memory size "
            << Q.get_device().get_info<info::device::local_mem_size>() / (1024.0)
            << " KiB" << std::endl;


  // initialize input and output memory on the host
  constexpr size_t sz = 256;

  // we could make the matrices non-square!
  constexpr size_t M = sz;
  constexpr size_t N = sz;
  constexpr size_t K = sz;

  // FIXME create operands
  std::vector<double> ...;

  // FIXME fill operands A and B with random numbers in the unit interval
  // hint: you call dist(mt) to get a random number
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // FIXME Create 2-dimensional buffers associated with inputs and output
  buffer<double, 2> ...;

  // Submit the kernel to the queue
  Q.submit([&](handler& cgh) {
    // FIXME define accessors for the buffers
    // hint: operands A and B are read-only, result C is write-only
    auto accA = accessor(...);
    auto accB = accessor(...);
    auto accC = accessor(...);

    // size of tile for loads from first operands
    // NOTE you can play around with the tile size
    constexpr auto tile_sz = 16;

    // FIXME define local accessor into work-group local memory
    // the constructor accepts a range and a handler
    auto tileA = ...;

    // FIXME declare global and local 2-dimensional ranges
    range global { ... };
    // hint: the local range will be used to index local memory, which is 1-dimensional
    range local { ... };

    // FIXME use nd_range with appropriate global and local ranges
    cgh.parallel_for(..., [=](nd_item<2> it) {
      // FIXME get indices in the global index space from the 2d item object
      // passed as input to the kernel
      // hint: these indices are used to address
      //  - the result (held in global memory)
      //  - the row of the left operand
      //  - the column of the right operand
      auto m = it. ...;
      auto n = it. ...;

      // FIXME get index in the local index space from the 2d item object
      // passed as input to the kernel
      // hint: this is used to address the local memory buffer
      auto i = it. ...;

      // accumulate result
      auto sum = 0.0;
      // loop over inner index (common to operands) with stride equal to the
      // tile size
      for (auto l = 0; l < K; l += tile_sz) {
        // FIXME load a tile of matrix A using the global memory accessor
        tileA[i] = ...;
        // synchronize to ensure all work-items have a consistent view of
        // the local memory holding the tile.
        // FIXME call the barrier
        ...;

        // loop over tile elements
        for (auto k = 0; k < tile_sz; ++k) {
          // FIXME load matrix B from its global memory accessor and perform multiplication
          sum += ...; 
        }

        // after computation, synchronize again, to ensure all
        // reads from the local memory tile are complete
        // FIXME call the barrier
        ...;
      }

      // FIXME write to the result matrix using its global memory accessor
      ...;
    });
  });

  // FIXME create a host_accessor to the result matrix
  host_accessor C{...};

  // Check that all outputs match serial execution
  bool passed = true;
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      double gold = 0.0;
      for (int k = 0; k < N; ++k) {
        gold += A[j * N + k] * B[k * N + i];
      }
      // NOTE we use double subscript for C since it's an accessor with a 2-dimensional range
      if (std::abs(gold - C[j][i]) / gold > 1.0e-12) {
        passed = false;
      }
    }
  }
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << std::endl;
  return (passed) ? 0 : 1;
}
