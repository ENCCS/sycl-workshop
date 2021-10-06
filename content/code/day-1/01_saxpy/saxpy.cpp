#include <cassert>
#include <iostream>
#include <vector>

#include <sycl/sycl.hpp>

using namespace sycl;

std::vector<float> saxpy(queue &Q, float alpha, const std::vector<float> &x,
                    const std::vector<float> &y) {
  assert(x.size() == y.size());

  std::vector<float> z(x.size());

  range<1> work_items{x.size()};

  {
    buffer<float> buff_x(x.data(), x.size());
    buffer<float> buff_y(y.data(), y.size());
    buffer<float> buff_z(z.data(), z.size());

    Q.submit([&](handler &cgh) {
      auto access_x = buff_x.get_access<access::mode::read>(cgh);
      auto access_y = buff_y.get_access<access::mode::read>(cgh);
      auto access_z = buff_z.get_access<access::mode::write>(cgh);

      cgh.parallel_for<class vector_add>(work_items, [=](id<1> tid) {
        access_z[tid] = alpha * access_x[tid] + access_y[tid];
      });
    });
  }

  return z;
}

int main() {
  const std::vector<float> a{1., 2., 3., 4., 5.};
  const std::vector<float> b{-1., 2., -3., 4., -5.};

  queue Q;

  std::cout << "Running on: " << Q.get_device().get_info<info::device::name>()
            << std::endl;

  auto c = saxpy(Q, 1.0, a, b);

  std::cout << "Result: " << std::endl;
  for (const auto x : c)
    std::cout << x << std::endl;
}
