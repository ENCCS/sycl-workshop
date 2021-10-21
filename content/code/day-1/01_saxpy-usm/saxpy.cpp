#include <cassert>
#include <iostream>
#include <numeric>
#include <vector>

#include <sycl/sycl.hpp>

using namespace sycl;

template <typename T>
std::vector<T>
saxpy(queue &Q, T alpha, const std::vector<T> &x, const std::vector<T> &y)
{
  assert(x.size() == y.size());
  auto sz = x.size();

  auto x_d = malloc_shared<T>(sz, Q);
  std::memcpy(x_d, x.data(), sizeof(T) * sz);

  auto y_d = malloc_shared<T>(sz, Q);
  std::memcpy(y_d, y.data(), sizeof(T) * sz);

  std::vector<T> z(sz);
  auto z_d = malloc_shared<T>(sz, Q);
  std::memcpy(z_d, z.data(), sizeof(T) * sz);

  Q.parallel_for(
     range { sz },
     [=](id<1> tid) {
       z_d[tid[0]] = alpha * x_d[tid[0]] + y_d[tid[0]];
     })
    .wait();

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

  auto c = saxpy(Q, 1.0, a, b);

  std::cout << "Checking results..." << std::endl;
  auto message = "Nice job!";
  for (const auto x : c) {
    if (std::abs(x - 1023.0) >= 1.0e-13) {
      std::cout << "Uh-oh!" << std::endl;
      message = "Not quite there yet :(";
    }
  }
  std::cout << message << std::endl;
}
