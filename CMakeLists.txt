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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(LAPACK CONFIG REQUIRED)
find_package(OpenBLAS CONFIG REQUIRED)
find_package(xtl CONFIG REQUIRED)
find_package(xtensor CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)

# Header-only library target
add_library(nconpp INTERFACE)
target_include_directories(nconpp INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/nconpp
)
target_link_libraries(nconpp INTERFACE
    OpenBLAS::OpenBLAS
    ${LAPACK_LIBRARIES}
    xtensor
    xtensor-blas
    $<$<NOT:$<BOOL:${SKBUILD}>>:xtensor::optimize>
)

# C++ tests
if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(CTest)
    include(tests/cpp/googletest.cmake)
endif()

# Python bindings
if(CMAKE_BUILD_TYPE MATCHES Python OR CMAKE_BUILD_TYPE MATCHES Debug OR SKBUILD)
    include(bindings/python/python.cmake)
endif()
