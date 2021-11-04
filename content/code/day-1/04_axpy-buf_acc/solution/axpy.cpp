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

  // buffer destructors are blocking. Scoping all SYCL work introduces an
  // implicit wait
  {
    // we can avoid using the second template parameter, as 1-dimensional
    // buffers are the default
    buffer<T, 1> bx(x.data(), work_items);
    buffer<T, 1> by(y.data(), work_items);
    buffer<T, 1> bz(z.data(), work_items);

    Q.submit([&](handler &cgh) {
      // access x: read-only and in global memory
      auto ax = accessor(bx, cgh, read_only);
      // access y: read-only and in global memory
      auto ay = accessor(by, cgh, read_only);
      // access z: write-only, in global memory, and discarding previous
      // contents
      auto az = accessor(bz, cgh, write_only, no_init);

      cgh.parallel_for(work_items, [=](id<1> tid) {
        az[tid] = alpha * ax[tid] + ay[tid];
      });
    });
  } // close the scope: we implicitly wait on the queue to complete

  // no need for an explicit copy
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
