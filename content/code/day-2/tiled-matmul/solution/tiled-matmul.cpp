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

  // initialize input and output memory on the host
  constexpr size_t sz = 256;

  // we could make the matrices non-square!
  constexpr size_t M = sz;
  constexpr size_t N = sz;
  constexpr size_t K = sz;

  std::vector<double> A(M * K), B(K * N), C(M * N);

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

  // zero-out c
  std::fill(C.begin(), C.end(), 0.0);

  {
    // Create buffers associated with inputs and output
    buffer<double, 2> bufA(A.data(), range<2>(M, K)),
      bufB(B.data(), range<2>(K, N)), bufC(C.data(), range<2>(M, N));

    // Submit the kernel to the queue
    Q.submit([&](handler& cgh) {
      auto accA = accessor(bufA, cgh, read_only);
      auto accB = accessor(bufB, cgh, read_only);
      auto accC = accessor(bufC, cgh, write_only, no_init);

      // size of tile for loads from first operands
      constexpr auto tile_sz = 16;

      // local accessor into work-group local memory
      auto tileA = accessor<double, 1, access_mode::read_write, target::local>(
        tile_sz, cgh);

      // declare global and local ranges
      // the global range spans the whole result matrix
      range global { M, N };
      // the local range spans one tile in the left operand
      // it really is a 1d range, though!
      range local { 1, tile_sz };

      cgh.parallel_for(nd_range { global, local }, [=](nd_item<2> it) {
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
          tileA[i] = accA[m][l + i];
          // synchronize to ensure all work-items have a consistent view of
          // the local memory holding the tile.
          it.barrier();

          // loop over tile elements
          for (auto k = 0; k < tile_sz; ++k) {
            // broadcast tile element to the subgroup and load matrix B from
            // global memory
            sum += tileA[k] * accB[l + k][n];
          }

          // after computation, synchronize again, to ensure all
          // reads from the local memory tile are complete
          it.barrier();
        }

        // finally, write to the result matrix
        accC[m][n] = sum;
      });
    });
  }

  // Check that all outputs match serial execution
  bool passed = true;
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      double gold = 0.0;
      for (int k = 0; k < N; ++k) {
        gold += A[j * N + k] * B[k * N + i];
      }
      if (std::abs(gold - C[j * N + i]) / gold > 1.0e-12) {
        passed = false;
      }
    }
  }
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << std::endl;
  return (passed) ? 0 : 1;
}
