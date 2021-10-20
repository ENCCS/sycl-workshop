#include <cassert>
#include <iostream>
#include <vector>

#include <sycl/sycl.hpp>

using namespace sycl;

std::vector<float>
saxpy(
  queue &Q,
  float alpha,
  const std::vector<float> &x,
  const std::vector<float> &y)
{
  assert(x.size() == y.size());
  auto sz = x.size();

  auto x_d = malloc_shared<float>(sz, Q);
  std::memcpy(x_d, x.data(), sz);

  auto y_d = malloc_shared<float>(sz, Q);
  std::memcpy(y_d, y.data(), sz);

  std::vector<float> z(sz, 0.0);
  auto z_d = malloc_shared<float>(z.size(), Q);
  std::memcpy(z_d, z.data(), sz);

  {
    Q.parallel_for(range<1> { sz }, [=](id<1> tid) {
      z_d[tid[0]] = alpha * x_d[tid[0]] + y_d[tid[0]];
    });
  }

  return z;
}

int
main()
{
  const std::vector<float> a { 1., 2., 3., 4., 5. };
  const std::vector<float> b { -1., 2., -3., 4., -5. };

  queue Q;

  std::cout << "Running on: " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  auto c = saxpy(Q, 1.0, a, b);

  std::cout << "Result: " << std::endl;
  for (const auto x : c)
    std::cout << x << std::endl;
}
