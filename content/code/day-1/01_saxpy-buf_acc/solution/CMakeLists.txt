cmake_minimum_required(VERSION 3.14)

project(saxpy LANGUAGES CXX)

# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# declare executable
add_executable(saxpy saxpy.cpp)

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(saxpy
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(saxpy
  PRIVATE
    -O3
  )

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(saxpy 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    saxpy 
  SOURCES 
    saxpy.cpp
  )
