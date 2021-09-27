#include <CL/sycl.hpp>

int main() {
  using namespace cl;

  auto defaultQueue = sycl::queue{};

  defaultQueue
      .submit([&](sycl::handler &cgh) {
        auto os = sycl::stream{128, 128, cgh};

        cgh.single_task<struct hello_world>([=]() { os << "Hello World!\n"; });
      })
      .wait();
}
