#include <cassert>
#include <iostream>

#include <CL/sycl.hpp>

std::vector<double> add(cl::sycl::queue &q, const std::vector<double> &a,
                        const std::vector<double> &b) {
  std::vector<double> c(a.size());

  assert(a.size() == b.size());

  cl::sycl::range<1> work_items{a.size()};

  {
    cl::sycl::buffer<double> buff_a(a.data(), a.size());
    cl::sycl::buffer<double> buff_b(b.data(), b.size());
    cl::sycl::buffer<double> buff_c(c.data(), c.size());

    q.submit([&](cl::sycl::handler &cgh) {
      auto access_a = buff_a.get_access<cl::sycl::access::mode::read>(cgh);
      auto access_b = buff_b.get_access<cl::sycl::access::mode::read>(cgh);
      auto access_c = buff_c.get_access<cl::sycl::access::mode::write>(cgh);

      cgh.parallel_for<class vector_add>(work_items, [=](cl::sycl::id<1> tid) {
        access_c[tid] = access_a[tid] + access_b[tid];
      });
    });
  }
  return c;
}

int main() {
  std::vector<double> a{1., 2., 3., 4., 5.};
  std::vector<double> b{-1., 2., -3., 4., -5.};

  cl::sycl::queue q;
  auto result = add(q, a, b);

  std::cout << "Result: " << std::endl;
  for (const auto x : result)
    std::cout << x << std::endl;
}
