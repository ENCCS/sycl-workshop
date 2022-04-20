cmake_minimum_required(VERSION 3.14)

project(heat LANGUAGES CXX C)

list(APPEND _sources 
  core.cpp
  io.cpp
  main.cpp
  setup.cpp
  utilities.cpp
  pngwriter.c
  )

add_executable(heat ${_sources})

# compile with ISO C++17
set(CMAKE_CXX_EXTENSIONS OFF)
target_compile_features(heat
  PRIVATE
    cxx_std_17
  )

# compile with optimizations on
target_compile_options(heat
  PRIVATE
    -O3
  )

find_package(PNG QUIET)
if(TARGET PNG::PNG)
  message(STATUS "Found PNG: enable saving time-evolution snapshots to PNG.")
  target_compile_definitions(heat
    PRIVATE
      HAVE_PNG
    )
  target_link_libraries(heat
    PRIVATE
      PNG::PNG
    )
endif()

# uncomment to use SYCL
# find hipSYCL compiler
find_package(hipSYCL CONFIG REQUIRED)

# find threading library...
find_package(Threads REQUIRED)
# ...and link against it
target_link_libraries(heat 
  PRIVATE 
    Threads::Threads
  )

# the SYCL secret sauce :)
add_sycl_to_target(
  TARGET 
    heat 
  SOURCES 
    ${_sources}
  )
