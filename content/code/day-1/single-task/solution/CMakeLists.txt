cmake_minimum_required(VERSION 3.14)

project(single-task LANGUAGES CXX)

# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# declare executable
add_executable(single-task single-task.cpp)

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(single-task
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(single-task
  PRIVATE
    -O3
  )

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(single-task 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    single-task 
  SOURCES 
    single-task.cpp
  )
