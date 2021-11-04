cmake_minimum_required(VERSION 3.14)

project(platform-devices LANGUAGES CXX)

# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# declare executable
add_executable(platform-devices platform-devices.cpp)

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(platform-devices
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(platform-devices
  PRIVATE
    -O3
  )

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(platform-devices 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    platform-devices 
  SOURCES 
    platform-devices.cpp
  )
