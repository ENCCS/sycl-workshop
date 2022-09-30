#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

// shortcuts for USM host and shared allocators classes
template <typename T>
using host_allocator = usm_allocator<T, usm::alloc::host>;
template <typename T>
using shared_allocator = usm_allocator<T, usm::alloc::shared>;

template <typename T>
std::vector<T, shared_allocator<T>>
axpy(
  queue& Q,
  T alpha,
  const std::vector<T, host_allocator<T>>& x,
  const std::vector<T, host_allocator<T>>& y)
{
  assert(x.size() == y.size());
  auto sz = x.size();

  auto ptrX = x.data();
  auto ptrY = y.data();

  std::vector z(sz, T { 0.0 }, shared_allocator<T>(Q));
  // get address of z, because we rely on by-copy capture in the kernel lambda.
  // Why?
  //  1. by-copy capture gives a const object in the kernel, resulting in a
  //  compiler error.
  //  2. by-reference capture gives a runtime error.
  auto ptrZ = z.data();

  Q.submit([&](handler& cgh) {
     cgh.parallel_for(range { sz }, [=](id<1> tid) {
       auto i  = tid[0];
       ptrZ[i] = alpha * ptrX[i] + ptrY[i];
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

  auto h_alloc = host_allocator<double>(Q);

  // fill vector x with 0, 1, 2, ..., sz-1
  std::vector x(sz, 0.0, h_alloc);
  std::iota(x.begin(), x.end(), 0.0);
  // fill vector y with sz-1, sz-2, ..., 1, 0
  std::vector y(sz, 0.0, h_alloc);
  std::iota(y.rbegin(), y.rend(), 0.0);

  auto z = axpy(Q, alpha, x, y);

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
}
