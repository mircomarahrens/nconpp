message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required (VERSION 3.16)

# vcpkg flags
set(VCPKG_FEATURE_FLAGS         "versions")

# cmake flags
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# perform a debug build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project(nconpp CXX)

# directories
set(NCONPP_CORE_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/core)
set(NCONPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
set(NCONPP_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/core/src)
set(NCONPP_UTILS_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/core/utils)
set(NCONPP_TEST_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(NCONPP_UT_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/test/ut)
set(NCONPP_PYTHON_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/py)

# collect dependencies
find_package(LAPACK       CONFIG REQUIRED)
find_package(OpenBLAS     CONFIG REQUIRED)
find_package(xtensor      CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)

set(NCONPP_DEPENDENCIES
    OpenBLAS::OpenBLAS
    xtensor
    xtensor-blas)

# core
add_subdirectory(${NCONPP_CORE_DIR})

# testing
if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    enable_testing()

    # GoogleTest
    find_package(GTest REQUIRED)

    add_subdirectory(${NCONPP_TEST_DIR})
endif()

# python module
add_subdirectory(${NCONPP_PYTHON_DIR})
