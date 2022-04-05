message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.16)

# avoid infinite recursion
if(DEFINED NCONPP_CORE_PROJECT)
    message(STATUS "NCONPP_CORE_PROJECT already called. Go to next stage.")
    return()
endif()

# vcpkg flags
set(VCPKG_FEATURE_FLAGS "versions")

# cmake flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(nconpp CXX)

set(NCONPP_LIBRARY_TARGET "nconpp")

# directories
set(NCONPP_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nconpp)
set(NCONPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nconpp/include)
set(NCONPP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nconpp/src)
set(NCONPP_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
set(NCONPP_UT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/ut)

find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
find_package(xtensor CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)

set(NCONPP_DEPENDENCIES
        ${BLAS_LIBRARIES}
        ${LAPACK_LIBRARIES}
        xtensor
        xtensor::optimize
        xtensor-blas)

# core
set(NCONPP_CORE_PROJECT ON)

set(NCONPP_MAIN
        ${NCONPP_CORE_DIR}/NetworkContractor.h
        ${NCONPP_CORE_DIR}/NetworkContractor.cpp)

set(NCONPP_INCLUDE
        ${NCONPP_INCLUDE_DIR}/Algorithms.h
        ${NCONPP_INCLUDE_DIR}/Container.h
        ${NCONPP_INCLUDE_DIR}/ErrorMessages.h
        ${NCONPP_INCLUDE_DIR}/Graph.h
        ${NCONPP_INCLUDE_DIR}/Network.h
        ${NCONPP_INCLUDE_DIR}/Tensor.h)

set(NCONPP_SRC
        ${NCONPP_SRC_DIR}/Algorithms.cpp
        ${NCONPP_SRC_DIR}/Graph.cpp
        ${NCONPP_SRC_DIR}/Network.cpp)

add_library(${NCONPP_LIBRARY_TARGET} SHARED
        ${NCONPP_MAIN}
        ${NCONPP_INCLUDE}
        ${NCONPP_SRC})

target_include_directories(${NCONPP_LIBRARY_TARGET} PRIVATE
        ${NCONPP_CORE_DIR}
        ${NCONPP_INCLUDE_DIR}
        ${NCONPP_SRC_DIR})

target_link_libraries(${NCONPP_LIBRARY_TARGET}
        ${NCONPP_DEPENDENCIES})


# testing
if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    # GoogleTest
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_subdirectory(${NCONPP_TEST_DIR})
endif ()
