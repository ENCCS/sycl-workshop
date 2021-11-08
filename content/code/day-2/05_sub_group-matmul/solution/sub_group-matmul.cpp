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
  // set up queue on any available device
  queue Q;

  std::cout << " Device " << Q.get_device().get_info<info::device::vendor>()
            << " " << Q.get_device().get_info<info::device::name>()
            << std::endl;

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

  std::vector<double> A(M * K), B(K * N);

  // fill a and b with random numbers in the unit interval
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  std::generate(A.begin(), A.end(), [&dist, &mt]() {
    return dist(mt);
  });
  std::generate(B.begin(), B.end(), [&dist, &mt]() {
    return dist(mt);
  });

  // Create buffers associated with inputs and output
  buffer<double, 2> bufA(A.data(), range<2>(M, K)),
    bufB(B.data(), range<2>(K, N)), bufC(range<2>(M, N));

  // Submit the kernel to the queue
  Q.submit([&](handler& cgh) {
    accessor accA { bufA, cgh, read_only };
    accessor accB { bufB, cgh, read_only };
    accessor accC { bufC, cgh, write_only, no_init };

    // NOTE we assume that the sub-group size is greater than or equal to the
    // tile size!
    constexpr auto tile_sz = 16;

    // declare global and local ranges
    // the global range spans the whole result matrix
    range global { M, N };
    // the local range spans one tile in the left operand
    // it really is a 1d range, because sub-groups are 1d!
    range local { 1, tile_sz };

    cgh.parallel_for(nd_range { global, local }, [=](nd_item<2> it) {
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
      for (auto l = 0; l < K; l += tile_sz) {
        // load a tile of matrix A
        auto tileA = accA[m][l + i];

        // loop over tile elements
        for (auto k = 0; k < tile_sz; ++k) {
          // broadcast tile element to the subgroup and load matrix B from
          // global memory
          sum += group_broadcast(sg, tileA, k) * accB[l + k][n];
        }
      }
      // finally, write to the result matrix
      accC[m][n] = sum;
    });
  });

  host_accessor C { bufC };

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
