#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

int
main()
{
  queue Q;

  std::cout << "we are running on: "
            << Q.get_device().get_info<info::device::vendor>() << " "
            << Q.get_device().get_info<info::device::name>() << std::endl;

  Q.submit([&](handler &cgh) {
	     auto out = sycl::stream(1024, 256, cgh);
	     cgh.single_task([=](){
			     out << "Hello from within a task!" << endl;
			     });
	   }).wait();

  return 0;
}
