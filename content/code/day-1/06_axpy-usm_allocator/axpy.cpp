#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

// shortcut for USM host allocator class
template <typename T>
using host_allocator = usm_allocator<T, usm::alloc::host>;
// FIXME define shortcut for USM shared allocator class
using shared_allocator = ...;

template <typename T>
// FIXME what should be the second template argument of the return type?
std::vector<T, ...>
axpy(
  queue& Q,
  T alpha,
  // FIXME what should be the second template argument of the input types?
  const std::vector<T, ...>& x,
  const std::vector<T, ...>& y)
{
  assert(x.size() == y.size());
  auto sz = x.size();

  // FIXME allocate output vector
  std::vector z(...);
  // NOTE why do we take the address of the output vector?
  auto ptrZ = z.data();

  Q.submit([&](handler& cgh) {
     cgh.parallel_for(range { sz }, [=](id<1> tid) {
       // FIXME implement AXPY kernel
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

  // FIXME create an allocator object on the queue for the operands.
  // Should it be host or shared?
  auto alloc = ...;

  // FIXME create and fill vector x with 0, 1, 2, ..., sz-1
  std::vector x(...);
  std::iota(x.begin(), x.end(), 0.0);
  // FIXME create and fill vector y with sz-1, sz-2, ..., 1, 0
  std::vector y(...);
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
