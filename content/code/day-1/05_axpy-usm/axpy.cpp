#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

template <typename T>
T*
axpy(queue& Q, size_t sz, T alpha, const T* x, const T* y)
{
  // FIXME allocate the result array
  // should it be host, device or shared?
  auto z = ...;

  Q.submit([&](handler& cgh) {
     cgh.parallel_for(range { sz }, [=](id<1> tid) {
       // FIXME define the AXPY kernel.
       // you need to extract an integer index from the id object. Hint: it can
       // be done with the subscript operator
     });
   })
    .wait();

  return z;
}

int
main()
{
  constexpr auto sz = 1024;

  constexpr auto alpha = 1.0;

  queue Q;

  std::cout << "Running on: " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  // FIXME allocate space for the x array
  // should it be host, device or shared?
  auto x = ...;
  // FIXME fill array with 0, 1, 2, ..., sz-1

  // FIXME allocate space for the y array
  // should it be host, device or shared?
  auto y = ...;
  // FIXME fill array with sz-1, sz-2, ..., 1, 0

  auto z = axpy(Q, sz, alpha, x, y);

  free(x, Q);
  free(y, Q);

  std::cout << "Checking results..." << std::endl;
  auto message = "Nice job!";
  for (auto i = 0; i < sz; ++i) {
    if (std::abs(z[i] - (sz - 1)) >= 1.0e-13) {
      std::cout << "Uh-oh!" << std::endl;
      std::cout << z[i] << std::endl;
      message = "Not quite there yet :(";
      break;
    }
  }
  std::cout << message << std::endl;

  free(z, Q);
}
