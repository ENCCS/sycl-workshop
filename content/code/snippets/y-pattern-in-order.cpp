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

  queue Q { property::queue::in_order() };

  auto A = malloc_shared<double>(N, Q);
  auto B = malloc_shared<double>(N, Q);

  // task A
  Q.parallel_for(range { N }, [=](id<1> id) {
    A[id[0]] = 1;
  });

  // task B
  Q.parallel_for(range { N }, [=](id<1> id) {
    B[id[0]] = 2;
  });

  // task C
  Q.parallel_for(range { N }, [=](id<1> id) {
    A[id[0]] += B[id[0]];
  });

  // task D
  Q.single_task([=]() {
    for (int i = 1; i < N; i++)
      A[0] += A[i];

    A[0] /= 3;
  });

  Q.wait();

  assert(A[0] == N);

  return EXIT_SUCCESS;
}
