message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required (VERSION 3.16)

# vcpkg flags
set(VCPKG_FEATURE_FLAGS         "versions")

# cmake flags
set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS  /MANIFEST:NO)
endif()

project(nconpp CXX)

# directories
set(NCONPP_CORE_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/core)
set(NCONPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
set(NCONPP_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/core/src)
set(NCONPP_UTILS_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/core/utils)
set(NCONPP_TEST_DIR    ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(NCONPP_UT_DIR      ${CMAKE_CURRENT_SOURCE_DIR}/test/ut)
set(NCONPP_PYTHON_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/nconpp-python)

# collect dependencies
get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
message(STATUS "Bin dir is: ${_VCPKG_INSTALLED_DIR}")

find_package(BLAS REQUIRED)
message(STATUS "BLAS LIBRARIES: " ${BLAS_LIBRARIES})
find_package(LAPACK REQUIRED)
message(STATUS "LAPACK LIBRARIES: ${LAPACK_LIBRARIES}")
find_package(xtensor      CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)

set(NCONPP_DEPENDENCIES
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    xtensor
    xtensor::optimize
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
