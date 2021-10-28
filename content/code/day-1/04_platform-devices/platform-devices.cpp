/* Adapted from: https://github.com/Apress/data-parallel-CPP
 *
 * Copyright (C) 2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

std::string
local_memory_type(const device& dev)
{

  switch (dev.get_info<info::device::local_mem_type>()) {
    case info::local_mem_type::local:
      return "local";
    case info::local_mem_type::global:
      return "global";
    case info::local_mem_type::none:
      return "none";
    default:
      return "none";
  }
}

int
main()
{
  // Loop through available platforms
  for (const auto& p : platform::get_platforms()) {
    std::cout << "Found platform: " << p.get_info<info::platform::name>()
              << std::endl;

    // Loop through available devices in this platform
    for (const auto& d : p.get_devices()) {
      // FIXME print vendor and name of device
      std::cout << " Device " << ... << std::endl;
      // FIXME print global memory size
      std::cout << "   Global memory size " << ... << std::endl;
      // FIXME print local memory type
      std::cout << "   Local memory type " << ... << std::endl;
      // FIXME print local memory size
      std::cout << "   Local memory size " << ... << std::endl;
      // FIXME print maximum work items dimensions
      std::cout << "   Maximum work items dimensions " << ... << std::endl;
      // FIXME print maximum work group size
      std::cout << "   Maximum work group size " << ... << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}
