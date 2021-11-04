#include <cassert>
#include <iostream>

#include <sycl/sycl.hpp>

using namespace sycl;

template <typename T>
T*
axpy(queue& Q, size_t sz, T alpha, const T* x, const T* y)
{
  auto z = malloc_shared<T>(sz, Q);

  Q.submit([&](handler& cgh) {
     cgh.parallel_for(range { sz }, [=](id<1> tid) {
       auto i = tid[0];
       z[i]   = alpha * x[i] + y[i];
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

  auto x = malloc_host<double>(sz, Q);
  // fill array with 0, 1, 2, ..., sz-1
  for (auto i = 0; i < sz; ++i) {
    x[i] = static_cast<double>(i);
  }

  auto y = malloc_host<double>(sz, Q);
  // fill array with sz-1, sz-2, ..., 1, 0
  for (auto i = sz - 1; i >= 0; --i) {
    y[i] = static_cast<double>((sz - 1) - i);
  }

  auto z = axpy(Q, sz, alpha, x, y);

  free(x, Q);
  free(y, Q);

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

  free(z, Q);
}
