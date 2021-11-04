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
      std::cout << " Device " << d.get_info<info::device::vendor>() << " "
                << d.get_info<info::device::name>() << std::endl;
      std::cout << "   Global memory size "
                << d.get_info<info::device::global_mem_size>() /
                     (1024.0 * 1024.0 * 1024.0)
                << " GiB" << std::endl;
      std::cout << "   Local memory type " << local_memory_type(d) << std::endl;
      std::cout << "   Local memory size "
                << d.get_info<info::device::local_mem_size>() / (1024.0)
                << " KiB" << std::endl;
      std::cout << "   Maximum work items dimesions "
                << d.get_info<info::device::max_work_item_dimensions>()
                << std::endl;
      std::cout << "   Maximum work group size "
                << d.get_info<info::device::max_work_group_size>() << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}
