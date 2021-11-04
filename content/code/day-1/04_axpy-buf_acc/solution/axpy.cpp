#include <cassert>
#include <iostream>
#include <vector>
#include <numeric>

#include <sycl/sycl.hpp>

using namespace sycl;

template <typename T>
std::vector<T>
axpy(
  queue &Q,
  T alpha,
  const std::vector<T> &x,
  const std::vector<T> &y)
{
  assert(x.size() == y.size());
  auto sz = x.size();

  std::vector<T> z(sz);

  range<1> work_items { sz };

  {
	  // we can avoid using the second template parameter, as 1-dimensional buffers are the default
    buffer<T, 1> buff_x(x.data(), work_items);
    buffer<T, 1> buff_y(y.data(), work_items);
    buffer<T, 1> buff_z(z.data(), work_items);

    Q.submit([&](handler &cgh) {
      auto access_x = accessor(buff_x, cgh, read_only);
      auto access_y = accessor(buff_y, cgh, read_only);
      auto access_z = accessor(buff_z, cgh, write_only, no_init);

      cgh.parallel_for<class vector_add>(work_items, [=](id<1> tid) {
        access_z[tid] = alpha * access_x[tid] + access_y[tid];
      });
    });
  }

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
