message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.20)

# avoid infinite recursion
if(DEFINED NCONPP_CORE_PROJECT)
    message(STATUS "NCONPP_CORE_PROJECT already called. Go next.")
    return()
endif()

#include(FetchContent)
#FetchContent_Declare(vcpkg
#        GIT_REPOSITORY https://github.com/microsoft/vcpkg.git
#        GIT_TAG 2022.03.10)
#FetchContent_GetProperties(vcpkg)
#if(NOT vcpkg_POPULATED)
#    FetchContent_Populate(vcpkg)
#endif()
# --no-warn-unused-cli -DCMAKE_TOOLCHAIN_FILE:FILEPATH=/home/mircomarahrens/vcpkg/scripts/buildsystems/vcpkg.cmake -G Ninja

# activate vcpkg manifest mode
set(VCPKG_FEATURE_FLAGS "versions")

# cmake flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(nconpp CXX)

set(NCONPP_LIBRARY_TARGET "nconpp")

# directories
set(NCONPP_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core)
set(NCONPP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core/include)
set(NCONPP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/core/src)

# dependencies
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

set(NCONPP_CORE
        ${NCONPP_CORE_DIR}/Nconpp.h
        ${NCONPP_CORE_DIR}/Nconpp.cpp)

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
        ${NCONPP_CORE}
        ${NCONPP_INCLUDE}
        ${NCONPP_SRC})

target_include_directories(${NCONPP_LIBRARY_TARGET} PRIVATE
        ${NCONPP_CORE_DIR}
        ${NCONPP_INCLUDE_DIR}
        ${NCONPP_SRC_DIR})

target_link_libraries(${NCONPP_LIBRARY_TARGET}
        ${NCONPP_DEPENDENCIES})

# test via gtest
if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    message(STATUS "GoogleTest process triggered.")
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

    set(NCONPP_TEST_TARGET "nconpp_test")

    set(NCONPP_TEST_MAIN
            test/nconpp_test.cpp)

    set(NCONPP_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test)
    set(NCONPP_UT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test/ut)

    set(NCONPP_TEST_UT
            ${NCONPP_UT_DIR}/NetworkTest.cpp
            ${NCONPP_UT_DIR}/NconppTest.cpp
            ${NCONPP_UT_DIR}/GraphTest.cpp
            ${NCONPP_UT_DIR}/TensorTest.cpp)

    add_executable(${NCONPP_TEST_TARGET}
            ${NCONPP_TEST_MAIN}
            ${NCONPP_TEST_UT})

    target_include_directories(${NCONPP_TEST_TARGET} PRIVATE
            ${NCONPP_CORE_DIR}
            ${NCONPP_INCLUDE_DIR}
            ${NCONPP_SRC_DIR})

    target_link_libraries(${NCONPP_TEST_TARGET}
            ${CMAKE_THREAD_LIBS_INIT}
            ${NCONPP_DEPENDENCIES}
            gtest_main
            nconpp)

    include(GoogleTest)
    gtest_discover_tests(${NCONPP_TEST_TARGET})
endif ()

# python via pybind
if (NOT CMAKE_BUILD_TYPE MATCHES Debug AND SKBUILD)
    message(STATUS "Skbuild process triggered.")

    set(PYBIND_MAIN pybind/main.cpp)
    set(PYBIND_MODULE _nconpp)

    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11
            GIT_TAG v2.9.1
    )

    FetchContent_Declare(
            xtensor-python
            GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-python
            GIT_TAG 0.25.3
    )

    FetchContent_MakeAvailable(pybind11 xtensor-python)

    set(Python_FIND_VIRTUALENV FIRST) # needed for poetry's virtualenv
    find_package(Python REQUIRED COMPONENTS Interpreter Development NumPy)
    message(STATUS "Use NumPy include dir: ${Python_NumPy_INCLUDE_DIRS}")

    pybind11_add_module(${PYBIND_MODULE}
            ${PYBIND_MAIN}
            ${NCONPP_INCLUDE}
            ${NCONPP_SRC})

    target_include_directories(${PYBIND_MODULE} PRIVATE
            ${NCONPP_CORE_DIR}
            ${NCONPP_INCLUDE_DIR}
            ${NCONPP_SRC_DIR}
            ${Python_NumPy_INCLUDE_DIRS})

    target_link_libraries(${PYBIND_MODULE} PRIVATE
            ${NCONPP_DEPENDENCIES}
            xtensor-python)

    target_compile_definitions(${PYBIND_MODULE} PRIVATE
            VERSION_INFO=${VERSION_INFO})

    #message(STATUS "PYTHON_LIBRARY_DIR is ${PYTHON_LIBRARY_DIR}")

    install(TARGETS ${PYBIND_MODULE}
            COMPONENT python
            LIBRARY DESTINATION .)
endif()