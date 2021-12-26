message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required (VERSION 3.16)

# build additional python module, TODO via env?
set(BUILD_PYTHON_MODULE         true)

# vcpkg flags
set(VCPKG_FEATURE_FLAGS         "versions")

# cmake flags
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

# windows specific, don't remove
#if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
#endif()

# perform a debug build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

project (nconpp CXX)

# directories
# core
set(NCONPP_DIR          ${CMAKE_CURRENT_SOURCE_DIR}/core)
set(NCONPP_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
set(NCONPP_SRC_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/core/src)
set(NCONPP_UTILS_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/core/utils)
# test
set(NCONPP_TEST_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/test)
# python
set(NCONPP_PYTHON_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/python)

# collect dependencies
find_package(LAPACK       CONFIG REQUIRED)
find_package(OpenBLAS     CONFIG REQUIRED)
find_package(xtensor      CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)

set(NCONPP_LIBRARIES
    OpenBLAS::OpenBLAS
    xtensor
    xtensor-blas)

# main
add_subdirectory(${NCONPP_DIR})

# testing
if(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()

    # GoogleTest
    find_package(GTest REQUIRED)

    add_subdirectory(${NCONPP_TEST_DIR})
endif()

# python module
if (BUILD_PYTHON_MODULE)
    add_subdirectory(${NCONPP_PYTHON_DIR})
endif()
