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

  queue hostQ{host_selector{}};
  std::cout << "host_selector, running on: " 
	  << hostQ.get_device().get_info<info::device::vendor>() << " "
	  << hostQ.get_device().get_info<info::device::name>()
            << std::endl;

  queue defaultQ{default_selector{}};
  std::cout << "default_selector, running on: " 
	  << defaultQ.get_device().get_info<info::device::vendor>() << " "
	  << defaultQ.get_device().get_info<info::device::name>()
            << std::endl;

  queue cpuQ{cpu_selector{}};
  std::cout << "cpu_selector, running on: "
	  << cpuQ.get_device().get_info<info::device::vendor>() << " " 
	  << cpuQ.get_device().get_info<info::device::name>() 
            << std::endl;

  queue gpuQ{gpu_selector{}};
  std::cout << "gpu_selector, running on: " 
	  << gpuQ.get_device().get_info<info::device::vendor>() << " "
	  << gpuQ.get_device().get_info<info::device::name>()
            << std::endl;

  // NOTE not available in hipSYCL 0.9.1
  //queue acceleratorQ{accelerator_selector{}};
  //std::cout << "accelerator_selector, running on: " 
  //           << acceleratorQ.get_device().get_info<info::device::vendor>() << " "
  //           << acceleratorQ.get_device().get_info<info::device::name>()
  //          << std::endl;

  return 0;
}
