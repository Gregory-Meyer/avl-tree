cmake_minimum_required(VERSION 3.1)

project(bloodhound VERSION 0.1.0)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include src)

add_library(bloodhound STATIC src/bit_stack.c src/map.c src/mem.c src/node.c
                              src/node_stack.c)

install(TARGETS bloodhound DESTINATION lib)
install(FILES include/bloodhound.h DESTINATION include)

option(BLOODHOUND_BUILD_TESTS "Build tests for libbloodhound." ON)
if(BLOODHOUND_BUILD_TESTS)
    enable_testing()

    add_subdirectory(./external/Catch2)

    include_directories(test)

    add_executable(test_bloodhound test/runner.cpp test/get.spec.cpp
                                   test/insert.spec.cpp test/remove.spec.cpp)
    target_link_libraries(test_bloodhound Catch2::Catch2 bloodhound)

    include(CTest)
    include(external/Catch2/contrib/Catch.cmake)
    catch_discover_tests(test_bloodhound)
endif()

option(BLOODHOUND_BUILD_DOCS "Build documentation for libbloodhound." ON)
if(BLOODHOUND_BUILD_DOCS)
    set(DOXYGEN_SKIP_DOT ON)
    find_package(Doxygen REQUIRED)

    set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in")
    set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

    configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

    add_custom_target(doc ALL "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating API documentation with Doxygen"
                      VERBATIM)

    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc/" DESTINATION "share/doc/${PROJECT_NAME}")
endif()
