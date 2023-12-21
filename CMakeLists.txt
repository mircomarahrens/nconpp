message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.20)

project(nconpp CXX)

# python
set(Python_FIND_VIRTUALENV FIRST)
set(Python_ROOT_DIR $ENV{PYTHON_VENV})
message(STATUS "Use Python venv: $ENV{PYTHON_VENV}")
find_package(Python COMPONENTS Interpreter Development.Module NumPy REQUIRED)


# cmake flags
set(CMAKE_CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF)

# git clone https://github.com/microsoft/vcpkg.git
# git submodule update --init --recursive
# activate vcpkg manifest mode
set(VCPKG_FEATURE_FLAGS "versions")

# dependencies
find_package(OpenBLAS CONFIG REQUIRED)
set(BLAS_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/openblas)
message(STATUS "cblas.h is located in " ${BLAS_INCLUDE_DIR})
message(STATUS "Blas libraries " ${BLAS_LIBRARIES})
find_package(LAPACK REQUIRED)

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
    xtensor-blas)

set(NCONPP_INCLUDE
    include/Graph.h
    include/LatticeGraph.h
    include/Tensor.h
    include/TensorNetwork.h
    )


# debug
if(CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    message(STATUS "GoogleTest process triggered.")
    project(nconpp_tests)

    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG v1.13.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(nconpp_tests_run
        test/nconpp_tests_run.cpp
        test/GraphTest.cpp
        test/LatticeGraphTest.cpp
        test/TensorTest.cpp
        test/TensorNetworkTest.cpp)

    target_include_directories(nconpp_tests_run PUBLIC
        include/
        ${BLAS_INCLUDE_DIR})

    target_link_libraries(nconpp_tests_run
        ${CMAKE_THREAD_LIBS_INIT}
        ${NCONPP_DEPENDENCIES}
        gmock_main
        gtest_main)

    # include(CTest)
    include(GoogleTest)
    gtest_discover_tests(nconpp_tests_run)
endif()


# python
project(nconpp_python LANGUAGES CXX)

# pybind11
add_subdirectory(python/extern/pybind11)

set(PYBIND11_NEWPYTHON ON)
set(PYBIND11_MAIN python/src/nconpp.cpp)
set(PYBIND11_MODULE _nconpp)

pybind11_add_module(${PYBIND11_MODULE}
    ${PYBIND11_MAIN}
    ${NCONPP_INCLUDE})

# xtensor-python
add_subdirectory(python/extern/xtensor-python)

target_include_directories(${PYBIND11_MODULE} PRIVATE
    include/
    python/extern/)

target_link_libraries(${PYBIND11_MODULE} PRIVATE
    ${NCONPP_DEPENDENCIES}
    pybind11::module
    xtensor-python)

if(CMAKE_BUILD_TYPE MATCHES Python AND SKBUILD)
    message(STATUS "Skbuild process triggered.")

    set(Python_FIND_VIRTUALENV FIRST)
    set(Python_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
    find_package(Python COMPONENTS Interpreter Development.Module NumPy REQUIRED)

    message(STATUS "Use NumPy include dir: ${Python_NumPy_INCLUDE_DIRS}")
    set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
    #message(STATUS "Use Python venv executable: ${Python_EXECUTABLE}")

#    get_cmake_property(_variableNames VARIABLES)
#    list (SORT _variableNames)
#    foreach (_variableName ${_variableNames})
#        message(STATUS "${_variableName}=${${_variableName}}")
#    endforeach()

    if(WIN32)
        set(VCPKG_INSTALLED_DIR_DLL_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/)

        if(EXISTS ${VCPKG_INSTALLED_DIR_DLL_DIR})
            message(STATUS "Found DLL directory: " ${VCPKG_INSTALLED_DIR_DLL_DIR})
        else()
            message(FATAL_ERROR "Could not find DLL directory.")
        endif()

        file(WRITE python/src/__init__.py
"import os, sys

if (sys.platform == 'win32'):
    os.add_dll_directory(\"${VCPKG_INSTALLED_DIR_DLL_DIR}\")

from ._nconpp import Graph, LatticeGraph, TensorNetwork

__all__ = (\"Graph\", \"LatticeGraph\", \"TensorNetwork\")
__version__ = \"0.1.0\"")
    endif()

    if (UNIX)
        file(WRITE python/src/__init__.py
"from ._nconpp import Graph, LatticeGraph, TensorNetwork

__all__ = (\"Graph\", \"LatticeGraph\", \"TensorNetwork\")
__version__ = \"0.1.0\"")
    endif()

    target_include_directories(${PYBIND11_MODULE} PRIVATE
        ${Python_NumPy_INCLUDE_DIRS}
        ${BLAS_INCLUDE_DIR})

    target_link_libraries(${PYBIND11_MODULE} PRIVATE
        Python::NumPy)

    target_compile_definitions(${PYBIND11_MODULE} PRIVATE
        VERSION_INFO=${VERSION_INFO})

    install(FILES python/src/__init__.py
        DESTINATION .)
    install(TARGETS ${PYBIND11_MODULE}
        LIBRARY DESTINATION .)
endif()
