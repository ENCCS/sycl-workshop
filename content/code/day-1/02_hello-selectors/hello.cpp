/* Adapted from: https://github.com/Apress/data-parallel-CPP
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

int
main()
{
  const std::string secret { "Ifmmp-!xpsme\"\012J(n!tpssz-!Ebwf/!"
                             "J(n!bgsbje!J!dbo(u!ep!uibu/!.!IBM\01" };

  const auto sz = secret.size();

  queue Q;

  std::cout << "Running on: " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  char *result = malloc_shared<char>(sz, Q);
  std::memcpy(result, secret.data(), sz);

  Q.parallel_for(
     range<1> { sz },
     [=](id<1> tid) {
       result[tid[0]] -= 1;
     })
    .wait();

  std::cout << result << "\n";

  free(result, Q);

  return 0;
}
