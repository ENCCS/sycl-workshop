#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include <sycl/sycl.hpp>

using namespace sycl;

template <typename T>
std::vector<T>
axpy(queue &Q, T alpha, const std::vector<T> &x, const std::vector<T> &y)
{
  assert(x.size() == y.size());

  // FIXME allocate array for x on the device
  auto x_d = ...;
  // FIXME copy x data to device
  std::memcpy(...);
  // FIXME allocate array for y on the device
  auto y_d = ...;
  // FIXME copy y data to device
  std::memcpy(...);

  // FIXME allocate result array
  auto z_d = ...;

  Q.parallel_for(
    ..., /* FIXME define execution range for the kernel */
    [=](id<1> tid) {
      ...; /* FIXME define AXPY action using x_d, y_d, alpha, and z_d */
    })
    /* FIXME should you wait? */;

  std::vector<T> z(sz);
  // FIXME copy result array into std::vector
  std::memcpy(...);

  // FIXME free ALL allocated memory
  free(...);

  return z;
}

int
main()
{
  constexpr auto sz = 1024;

  // fill vector a with 0, 1, 2, ..., sz-1
  std::vector<double> a(sz, 0.0);
  std::iota(a.begin(), a.end(), 0.0);
  // fill vector b with sz-1, sz-2, ..., 1, 0
  std::vector<double> b(sz, 0.0);
  std::iota(b.rbegin(), b.rend(), 0.0);

  queue Q;

  std::cout << "Running on: " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  auto c = axpy(Q, 1.0, a, b);

  std::cout << "Checking results..." << std::endl;
  auto message = "Nice job!";
  for (const auto x : c) {
    if (std::abs(x - 1023.0) >= 1.0e-13) {
      std::cout << "Uh-oh!" << std::endl;
      message = "Not quite there yet :(";
      break;
    }
  }
  std::cout << message << std::endl;
}
