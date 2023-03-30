message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.20)

project(nconpp CXX)

# git clone https://github.com/microsoft/vcpkg.git
# activate vcpkg manifest mode
set(VCPKG_FEATURE_FLAGS "versions")

# cmake flags
set(CMAKE_CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED ON
    CMAKE_CXX_EXTENSIONS OFF)

set(Boost_NO_WARN_NEW_VERSIONS 1)

# dependencies
find_package(Boost REQUIRED graph)
find_package(BLAS REQUIRED)
set(BLAS_INCLUDE_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/openblas)
message(STATUS "cblas.h is located in " ${BLAS_INCLUDE_DIR})
find_package(LAPACK REQUIRED)
find_package(xtensor CONFIG REQUIRED)
find_package(xtensor-blas CONFIG REQUIRED)

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

include(FetchContent)

# debug
if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    message(STATUS "GoogleTest process triggered.")
    project(nconpp_tests)

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
if (NOT CMAKE_BUILD_TYPE MATCHES Debug AND SKBUILD)
    message(STATUS "Skbuild process triggered.")

    set(PYBIND_MAIN python/main.cpp)
    set(PYBIND_MODULE _nconpp)

    include(FetchContent)
    FetchContent_Declare(
            pybind11
            GIT_REPOSITORY https://github.com/pybind/pybind11
            GIT_TAG v2.10.4
    )

    FetchContent_Declare(
            xtensor-python
            GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-python
            GIT_TAG 0.26.1
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

    message(STATUS "PYTHON_LIBRARY_DIR is ${PYTHON_LIBRARY_DIR}")

    install(TARGETS ${PYBIND_MODULE}
            COMPONENT python
            LIBRARY DESTINATION .)
endif()