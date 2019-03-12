cmake_minimum_required(VERSION 3.1)

project(bloodhound)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include src)

add_library(bloodhound STATIC src/map.c src/mem.c src/node_stack.c)

option(AVLBST_BUILD_TESTS "Build tests for libbloodhound." ON)
if(AVLBST_BUILD_TESTS)
    enable_testing()

    find_package(Catch2 REQUIRED)

    add_executable(test_bloodhound test/runner.cpp test/get.spec.cpp test/insert.spec.cpp)
    target_link_libraries(test_bloodhound Catch2::Catch2 bloodhound)

    include(CTest)
    include(Catch)
    catch_discover_tests(test_bloodhound)
endif()
