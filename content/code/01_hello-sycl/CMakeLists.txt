cmake_minimum_required (VERSION 3.14)

project(hello 
	LANGUAGES CXX)

find_package(Threads REQUIRED)

find_package(hipSYCL CONFIG REQUIRED)

add_executable(hello hello.cpp)

target_compile_definitions(hello 
  PUBLIC
    SYCL_LANGUAGE_VERSION=2020
  )

target_link_libraries(hello 
  PRIVATE 
    Threads::Threads
  )

set_target_properties(hello 
  PROPERTIES 
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )

