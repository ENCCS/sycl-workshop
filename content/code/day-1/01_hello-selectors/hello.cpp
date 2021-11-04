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

  // FIXME define a queue with the host selector
  queue hostQ{ ... };
  // FIXME print out vendor and name of selected device
  std::cout << "host_selector, running on: " << hostQ.get_device().get_info<...>()            << std::endl;

  // FIXME define a queue with the default selector
  queue defaultQ{ ... ;
  // FIXME print out vendor and name of selected device

  // FIXME define a queue with the default selector
  queue defaultQ{ ... ;
  // FIXME print out vendor and name of selected device

  // FIXME define a queue with the CPU selector
  queue cpuQ{ ... };
  // FIXME print out vendor and name of selected device

  // FIXME define a queue with the GPU selector
  queue gpuQ{ ... };
  // FIXME print out vendor and name of selected device

  return 0;
}
