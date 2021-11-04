/* Adapted from: https://github.com/Apress/data-parallel-CPP
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

class custom_selector : public device_selector {
  public:
    int operator()(const device &dev) const override {
      int score = -1;

      if (dev.is_gpu()) {
	// FIXME get vendor name
	auto vendor = ...;
	// FIXME score AMD GPU

	// FIXME score Nvidia GPU

	// FIXME score Intel GPU
      }

      // FIXME what happens if no GPU is found?
      // should we score the CPU so that we have a fallback option?

      return score;
    }
};

int
main()
{
  std::cout << "Give me any GPU!" << std::endl;

  auto Q = queue{ custom_selector{} };

  std::cout << "we are running on: "
    << Q.get_device().get_info<info::device::vendor>() << " " 
    << Q.get_device().get_info<info::device::name>() << std::endl; 

  return 0;
}
