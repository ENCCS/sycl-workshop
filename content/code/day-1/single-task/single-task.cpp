#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

int
main()
{
  // FIXME create a queue object

  std::cout << "we are running on: "
            << Q.get_device().get_info<info::device::vendor>() << " "
            << Q.get_device().get_info<info::device::name>() << std::endl;

  // FIXME submit work with a command group handler
  Q.submit(... {
     // FIXME create a stream object
     auto out = stream(...);
     // FIXME create a single task in the command group handler
     cgh.single_task(...);
   });
  // FIXME should we wait?

  return 0;
}
