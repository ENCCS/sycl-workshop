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

int
main()
{
  // FIXME set up queue on GPU

  std::cout << " Device " << Q.get_device().get_info<info::device::vendor>()
            << " " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  // find out info on sub-groups for the current device
  std::cout << " Subgroup sizes: ";
  for (const auto& x :
       Q.get_device().get_info<info::device::sub_group_sizes>()) {
    std::cout << x << " ";
  }
  std::cout << std::endl;

  // initialize input and output memory on the host
  constexpr size_t sz = 256;

  // we could make the matrices non-square!
  constexpr size_t M = sz;
  constexpr size_t N = sz;
  constexpr size_t K = sz;

  // FIXME create A and B operands of sizes MxK and KxN
  std::vector<double> ...;

  // FIXME fill operands with random numbers in the unit interval
  // hint: you call dist(mt) to get one random number
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  // FIXME Create buffers associated with input matrices
  buffer<double, 2> bufA(...), bufB(...);
  // FIXME create buffer for output matrix
  // hint: we will use a host accessor to retrieve the data, so only the range
  // argument is needed!
  buffer<double, 2> bufC(...);

  // Submit the kernel to the queue
  Q.submit([&](handler& cgh) {
    // FIXME define accessort for the buffers
    // hint: A and B are read-only, while C is write-only and we don't care
    // about any value initially present in its memory.
    accessor accA { ... };
    accessor accB { ... };
    accessor accC { ... };

    // NOTE we assume that the sub-group size is greater than or equal to the
    // tile size!
    constexpr auto tile_sz = 16;

    // FIXME declare global and local ranges
    // hint: the global range spans the whole result matrix
    range global { ... };
    // hint: the local range spans one tile in the left operand
    // it really is a 1d range, because sub-groups are 1d!
    range local { ... };

    cgh.parallel_for(..., [=](...) {
      // get subgroup
      auto sg = it.get_sub_group();

      // indices in the global index space
      // these are used to address
      //  - the result (held in global memory)
      //  - the row of the left operand
      //  - the column of the right operand
      auto m = it.get_global_id()[0];
      auto n = it.get_global_id()[1];

      // index in the local index space
      auto i = it.get_local_id()[1];

      // accumulate result
      auto sum = 0.0;
      // loop over inner index (common to operands) with stride equal to the
      // tile size
      for (decltype(K) l = 0; l < K; l += tile_sz) {
        // FIXME load a tile of matrix A, i.e. tile_sz elements in the m-th row
        auto tileA = accA[...][...];

        // loop over tile elements
        for (decltype(tile_sz) k = 0; k < tile_sz; ++k) {
          // FIXME: broadcast tile element to the subgroup and load matrix B from
          // global memory, i.e. tile_sz rows in column k
          sum += group_broadcast(sg, tileA, k) * accB[...][...];
        }
      }
      // FIXME finally, write to the result matrix
      accC[...][...] = sum;
    });
  });

  // FIXME trigger copy back to host of the result matrix using a host accessor
  host_accessor C{...};

  // Check that all outputs match serial execution
  bool passed = true;
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      double gold = 0.0;
      for (int k = 0; k < N; ++k) {
        gold += A[j * N + k] * B[k * N + i];
      }
      if (std::abs(gold - C[j][i]) / gold > 1.0e-12) {
        passed = false;
      }
    }
  }
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << std::endl;
  return (passed) ? 0 : 1;
}
