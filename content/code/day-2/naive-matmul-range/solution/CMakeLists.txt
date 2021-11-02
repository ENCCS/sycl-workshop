cmake_minimum_required(VERSION 3.14)

project(naive-matmul-range LANGUAGES CXX)

# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# declare executable
add_executable(naive-matmul-range naive-matmul-range.cpp)

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(naive-matmul-range
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(naive-matmul-range
  PRIVATE
    -O3
  )

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(naive-matmul-range 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    naive-matmul-range 
  SOURCES 
    naive-matmul-range.cpp
  )
