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
  auto sz = x.size();

  std::vector<T> z(sz);

  range<1> work_items { sz };

  // FIXME declare buffer to view into x
  buffer<T> bx(..., ...);
  // FIXME declare buffer to view into y
  // FIXME declare buffer to view into z

  Q.submit([&](handler &cgh) {
    // FIXME declare accessor into bx. read-only and in global memory
    auto ax = accessor(...);
    // FIXME declare accessor into by. read-only and in global memory
    // FIXME declare accessor into bz. write-only and in global memory

    cgh.parallel_for(work_items, [=](id<1> tid) {
      // FIXME perform AXPY operation. Hint: we can index accessor directly with an id object
      az[tid] = ...;
    });
  });
  // FIXME make sure the vector z on the host is up-to-date

  // no need for an explicit copy
  return z;
}

int
main()
{
  constexpr auto sz = 1024;

  // fill vector a with 0, 1, 2, ..., sz-1
  std::vector a(sz, 0.0);
  std::iota(a.begin(), a.end(), 0.0);
  // fill vector b with sz-1, sz-2, ..., 1, 0
  std::vector b(sz, 0.0);
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
