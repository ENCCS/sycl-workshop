class special_device_selector : public device_selector
{
public:
  int operator()(const sycl::device& dev) const override
  {
    if (dev.is_gpu()) {
      auto vendorName = dev.get_info<sycl::info::device::vendor>();
      if (vendorName.find("Intel") != std::string::npos) {
        return 1;
      }
    }
    return -1;
  }
};

auto Q = queue{ special_device_selector {} };
