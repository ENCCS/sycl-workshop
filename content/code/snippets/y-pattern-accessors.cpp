/* Adapted from: https://github.com/Apress/data-parallel-CPP
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <cassert>
#include <cstdlib>
#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

int
main()
{
  constexpr auto N = 256;

  // set up queue on any available device
  queue Q;

  buffer<double> A { range { N } };
  buffer<double> B { range { N } };

  Q.submit([&](handler &h) {
    accessor aA { A, h };
    h.parallel_for(range { N }, [=](id<1> id) {
      aA[id] = 1;
    });
  });

  Q.submit([&](handler &h) {
    accessor aB { B, h };
    h.parallel_for(range { N }, [=](id<1> id) {
      aB[id] = 2;
    });
  });

  Q.submit([&](handler &h) {
    accessor aA { A, h };
    accessor aB { B, h, read_only };
    h.parallel_for(range { N }, [=](id<1> id) {
      aA[id] += aB[id];
    });
  });

  Q.submit([&](handler &h) {
    accessor aA { A, h };
    h.single_task([=]() {
      for (int i = 1; i < N; i++)
        aA[0] += aA[i];

      aA[0] /= 3;
    });
  });

  host_accessor h_a { A };
  assert(h_a[0] == N);

  return EXIT_SUCCESS;
}
