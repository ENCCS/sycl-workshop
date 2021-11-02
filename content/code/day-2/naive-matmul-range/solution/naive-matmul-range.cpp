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

int main() {
  // set up queue on any available device
  queue Q;

  // initialize input and output memory on the host
  constexpr size_t N = 256;
  std::vector<double> a(N * N), b(N * N), c(N * N);

// fill a and b with random numbers in the unit interval
std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(0.0, 1.0);

std::generate(a.begin(), a.end(), [&dist, &mt]() { return dist(mt); });
std::generate(b.begin(), b.end(), [&dist, &mt]() { return dist(mt); });

// zero-out c
  std::fill(c.begin(), c.end(), 0.0);

  {
    // Create buffers associated with inputs and output
    buffer<double, 2> a_buf(a.data(), range<2>(N, N)),
        b_buf(b.data(), range<2>(N, N)), c_buf(c.data(), range<2>(N, N));

    // Submit the kernel to the queue
    Q.submit([&](handler& cgh) {
      accessor a{a_buf, cgh};
      accessor b{b_buf, cgh};
      accessor c{c_buf, cgh};

      cgh.parallel_for(range{N, N}, [=](id<2> idx) {
        int j = idx[0];
        int i = idx[1];
        for (int k = 0; k < N; ++k) {
          c[j][i] += a[j][k] * b[k][i]; 
	  // we can use the id object directly:
	  // c[idx] += a[id(j,k)] * b[id(k,i)];
        }
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
