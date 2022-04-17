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

  std::cout << "Running on: " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  // initialize input and output memory on the host
  constexpr size_t N = 256;
  // FIXME create operands and result of N*N size
  std::vector<double> ...;

  // fill operands with random numbers in the unit interval
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0.0, 1.0);

  std::generate(a.begin(), a.end(), [&dist, &mt]() {
    return dist(mt);
  });
  std::generate(b.begin(), b.end(), [&dist, &mt]() {
    return dist(mt);
  });

  // FIXME zero-out c
  std::fill(...);

  {
    // FIXME create 2-dimensional buffers associated with inputs and output
    buffer<double, 2> ...;

    // Submit the kernel to the queue
    Q.submit([&](handler& cgh) {
      // FIXME define accessors for the buffers
      accessor a { ... };
      accessor b { ... };
      accessor c { ... };

      cgh.parallel_for(
        // FIXME use an appropriate 2-dimensional range
        ..., [=](...) {
          // FIXME implement the matrix multiplication kernel
      });
    });
  }

  // Check that all outputs match serial execution
  bool passed = true;
  for (int j = 0; j < N; ++j) {
    for (int i = 0; i < N; ++i) {
      double gold = 0;
      for (int k = 0; k < N; ++k) {
        gold += a[j * N + k] * b[k * N + i];
      }
      if (std::abs(gold - c[j * N + i]) / gold > 1.0e-12) {
        passed = false;
      }
    }
  }
  std::cout << ((passed) ? "SUCCESS" : "FAILURE") << std::endl;
  return (passed) ? 0 : 1;
}
