message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.28)

project(nconpp CXX)

# cmake flags
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# git clone https://github.com/microsoft/vcpkg.git
# git submodule update --init --recursive
# activate vcpkg manifest mode
set(VCPKG_FEATURE_FLAGS "versions")

set(NCONPP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(NCONPP_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(NCONPP_PY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/misc/python)

# dependencies
find_package(LAPACK CONFIG REQUIRED)
find_package(OpenBLAS CONFIG REQUIRED)
set(BLAS_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/openblas)
message(STATUS "cblas.h is located in " ${BLAS_INCLUDE_DIR})
message(STATUS "Blas libraries " ${BLAS_LIBRARIES})

find_package(xtl CONFIG REQUIRED)
find_package(xtensor CONFIG REQUIRED)
message(STATUS "xtensor version is " ${xtensor_VERSION} ".")
find_package(xtensor-blas REQUIRED)
message(STATUS "xtensor-blas version is " ${xtensor-blas_VERSION} ".")

set(NCONPP_DEPENDENCIES
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    xtensor
    xtensor::optimize
    xtensor-blas
    )

set(NCONPP_SRC
    src/Graph.hpp
    src/LatticeGraph.hpp
    src/Tensor.hpp
    src/TensorNetwork.hpp
    )

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# debug
if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    include(cmake/googletest.cmake)
endif()

# python
if (CMAKE_BUILD_TYPE MATCHES Python OR Debug)
    include(cmake/python.cmake)
endif()
