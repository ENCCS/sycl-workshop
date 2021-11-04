/* Adapted from: https://github.com/Apress/data-parallel-CPP
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

class custom_selector : public device_selector
{
public:
  int operator()(const device &dev) const override
  {
    int score = -1;

    if (dev.is_gpu()) {
      auto vendor = dev.get_info<info::device::vendor>();
      if (vendor.find("AMD") != std::string::npos) score += 75;

      if (vendor.find("NVIDIA") != std::string::npos) score += 50;

      if (vendor.find("Intel") != std::string::npos) score += 100;
    }

    // Give host device points so it is used if no GPU is available.
    // Without these next two lines, systems with no GPU would select
    // nothing, since we initialize the score to a negative number above.
    if (dev.is_host()) score += 25;

    return score;
  }
};

int
main()
{
  std::cout << "Give me any GPU!" << std::endl;

  auto Q = queue { custom_selector {} };

  std::cout << "we are running on: "
            << Q.get_device().get_info<info::device::vendor>() << " "
            << Q.get_device().get_info<info::device::name>() << std::endl;

  return 0;
}
