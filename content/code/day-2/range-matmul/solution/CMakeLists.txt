cmake_minimum_required(VERSION 3.14)

project(range-matmul LANGUAGES CXX)

# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# declare executable
add_executable(range-matmul range-matmul.cpp)

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(range-matmul
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(range-matmul
  PRIVATE
    -O3
  )

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(range-matmul 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    range-matmul 
  SOURCES 
    range-matmul.cpp
  )
