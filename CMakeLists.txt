message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.20)

project(nconpp CXX)

# cmake flags
set(CMAKE_CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF)

# git clone https://github.com/microsoft/vcpkg.git
# activate vcpkg manifest mode
set(VCPKG_FEATURE_FLAGS "versions")

set(Boost_NO_WARN_NEW_VERSIONS 1)

# dependencies
find_package(Boost REQUIRED graph)
find_package(BLAS REQUIRED)
set(BLAS_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/openblas)
message(STATUS "cblas.h is located in " ${BLAS_INCLUDE_DIR})
message(STATUS "Blas libraries " ${BLAS_LIBRARIES})
find_package(LAPACK REQUIRED)

find_package(XTENSOR REQUIRED)
message(STATUS "xtensor version is " ${XTENSOR_VERSION} ".")
find_package(XTENSOR-BLAS REQUIRED)
message(STATUS "xtensor-blas version is " ${XTENSOR-BLAS_VERSION} ".")

set(NCONPP_DEPENDENCIES
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
    xtensor
    xtensor::optimize
    xtensor-blas)

set(NCONPP_INCLUDE
    include/LogMessages.h
    include/Tensor.h
    include/TensorNetwork.h
    )


        # debug
if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
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
        test/TensorNetworkTest.cpp
        test/TensorTest.cpp)

    target_include_directories(nconpp_tests_run PUBLIC
        include/
        ${BLAS_INCLUDE_DIR})

    target_link_libraries(nconpp_tests_run
        ${CMAKE_THREAD_LIBS_INIT}
        ${NCONPP_DEPENDENCIES}
        gmock_main
        gtest_main)
endif ()

# python
if (CMAKE_BUILD_TYPE MATCHES Python AND SKBUILD)
    message(STATUS "Skbuild process triggered.")

    project(nconpp_python LANGUAGES CXX)

    set(Python_FIND_VIRTUALENV FIRST)
    set(Python_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/venv)
    find_package(Python COMPONENTS Interpreter Development.Module NumPy REQUIRED)

    message(STATUS "Use NumPy include dir: ${Python_NumPy_INCLUDE_DIRS}")
    set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
    message(STATUS "Use Python venv executable: ${Python_EXECUTABLE}")

#    get_cmake_property(_variableNames VARIABLES)
#    list (SORT _variableNames)
#    foreach (_variableName ${_variableNames})
#        message(STATUS "${_variableName}=${${_variableName}}")
#    endforeach()

    add_subdirectory(python/extern/pybind11)

    if (WIN32)
        set(VCPKG_INSTALLED_DIR_DLL_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/)

        if (EXISTS ${VCPKG_INSTALLED_DIR_DLL_DIR})
            message(STATUS "Found DLL directory: " ${VCPKG_INSTALLED_DIR_DLL_DIR})
        else() 
            message(FATAL_ERROR "Could not find DLL directory.")
        endif()

        file(WRITE python/__init__.py 
"import os, sys

if (sys.platform == 'win32'):
    os.add_dll_directory(\"${VCPKG_INSTALLED_DIR_DLL_DIR}\")")
    endif()

    # pybind11
    set(PYBIND11_NEWPYTHON ON)
    set(PYBIND11_MAIN python/main.cpp)
    set(PYBIND11_MODULE nconpp)

    add_subdirectory(python/extern/xtensor-python)

    pybind11_add_module(${PYBIND11_MODULE}
        ${PYBIND11_MAIN}
        ${NCONPP_INCLUDE})

    target_include_directories(${PYBIND11_MODULE} PRIVATE
        include/
        ${Python_NumPy_INCLUDE_DIRS}
        ${BLAS_INCLUDE_DIR})

    target_link_libraries(${PYBIND11_MODULE} PRIVATE
        ${NCONPP_DEPENDENCIES}
        pybind11::module
        xtensor-python
        Python::NumPy)

    target_compile_definitions(${PYBIND11_MODULE} PRIVATE
        VERSION_INFO=${VERSION_INFO})

    install(FILES python/__init__.py DESTINATION .)
    install(TARGETS ${PYBIND11_MODULE}
        LIBRARY DESTINATION .)
endif()