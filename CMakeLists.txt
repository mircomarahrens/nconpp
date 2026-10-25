message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.28)

# Set overlay triplets and toolchain file for vcpkg
if(NOT DEFINED VCPKG_OVERLAY_TRIPLETS)
    set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/triplets" CACHE STRING "Overlay triplets directory for vcpkg")
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    else()
        set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

project(nconpp LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

include(CTest)

# cmake flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# git clone https://github.com/microsoft/vcpkg.git
# git submodule update --init --recursive
# activate vcpkg manifest mode
set(VCPKG_FEATURE_FLAGS "versions")

set(NCONPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(NCONPP_TESTS_CPP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp)
set(NCONPP_BINDINGS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bindings)
set(NCONPP_EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)

# dependencies
find_package(LAPACK CONFIG REQUIRED)
find_package(OpenBLAS CONFIG REQUIRED)
set(BLAS_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/openblas)
message(STATUS "cblas.h is located in " ${BLAS_INCLUDE_DIR})
message(STATUS "Blas libraries " ${BLAS_LIBRARIES})

find_package(xtl CONFIG REQUIRED)
find_package(xtensor CONFIG REQUIRED)
message(STATUS "xtensor version is " ${xtensor_VERSION} ".")
find_package(xtensor-blas CONFIG REQUIRED)
message(STATUS "xtensor-blas version is " ${xtensor-blas_VERSION} ".")

set(NCONPP_DEPENDENCIES
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    xtensor
    xtensor-blas
    )
if(NOT SKBUILD)
    list(APPEND NCONPP_DEPENDENCIES xtensor::optimize)
endif()

set(NCONPP_CPP_HEADERS
    ${NCONPP_INCLUDE_DIR}/nconpp/ErrorMessages.hpp
    ${NCONPP_INCLUDE_DIR}/nconpp/Graph.hpp
    ${NCONPP_INCLUDE_DIR}/nconpp/LatticeGraph.hpp
    ${NCONPP_INCLUDE_DIR}/nconpp/LinAlg.hpp
    ${NCONPP_INCLUDE_DIR}/nconpp/Tensor.hpp
    ${NCONPP_INCLUDE_DIR}/nconpp/TensorNetwork.hpp
    ${NCONPP_INCLUDE_DIR}/nconpp/Utils.hpp
    )

# debug / C++ tests
if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(tests/cpp/googletest.cmake)
endif()

# python bindings
if(CMAKE_BUILD_TYPE MATCHES Python OR SKBUILD)
    include(bindings/python/python.cmake)
endif()
