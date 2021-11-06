cmake_minimum_required(VERSION 3.14)

project(nd_range-matmul LANGUAGES CXX)

# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# declare executable
add_executable(nd_range-matmul nd_range-matmul.cpp)

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(nd_range-matmul
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(nd_range-matmul
  PRIVATE
    -O3
  )

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(nd_range-matmul 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    nd_range-matmul 
  SOURCES 
    nd_range-matmul.cpp
  )
