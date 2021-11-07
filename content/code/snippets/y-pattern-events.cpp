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

  auto a = malloc_shared<double>(N, Q);
  auto b = malloc_shared<double>(N, Q);

  auto e1 = Q.parallel_for(range { N }, [=](id<1> id) {
    a[id[0]] = 1;
  });

  auto e2 = Q.parallel_for(range { N }, [=](id<1> id) {
    b[id[0]] = 2;
  });

  auto e3 = Q.parallel_for(range { N }, { e1, e2 }, [=](id<1> id) {
    a[id[0]] += b[id[0]];
  });

  Q.single_task(e3, [=]() {
    for (int i = 1; i < N; i++)
      data1[0] += data1[i];

    data1[0] /= 3;
  });

  Q.wait();

  assert(a[0] == N);

  return EXIT_SUCCESS;
}
