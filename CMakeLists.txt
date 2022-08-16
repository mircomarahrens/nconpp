message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.20)

# avoid infinite recursion
if(DEFINED NCONPP_CORE_PROJECT)
    message(STATUS "NCONPP_CORE_PROJECT already called. Go next.")
    return()
endif()

# cmake flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(nconpp CXX)

set(NCONPP_CORE_PROJECT ON)

set(NCONPP_LIB
        lib/Nconpp.h
        lib/Nconpp.cpp)

set(NCONPP_INCLUDE
        include/ErrorMessages.h
        include/Graph.h
        include/Tensor.h
        include/TensorNetwork.h
        )

set(NCONPP_SRC
        src/Graph.cpp
        src/Tensor.cpp
        src/TensorNetwork.cpp
        )

add_library(nconpp SHARED
        ${NCONPP_LIB}
        ${NCONPP_INCLUDE}
        ${NCONPP_SRC})

target_include_directories(nconpp PRIVATE
        lib/
        include/
        src/)

# test via gtest
if (CMAKE_BUILD_TYPE MATCHES Debug AND NOT SKBUILD)
    message(STATUS "GoogleTest process triggered.")
    # GoogleTest: git clone https://github.com/google/googletest.git
    add_subdirectory(googletest)
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    enable_testing()

    add_executable(nconpp_test
            test/nconpp_test.cpp
            test/ut/NconppTest.cpp
            test/ut/GraphTest.cpp
            test/ut/TensorTest.cpp
            test/ut/TensorNetworkTest.cpp)

    target_include_directories(nconpp_test PUBLIC
            lib/
            include/
            src/)

    target_link_libraries(nconpp_test
            ${CMAKE_THREAD_LIBS_INIT}
            gtest
            gtest_main
            nconpp)

    include(GoogleTest)
    gtest_discover_tests(nconpp_test)
endif ()

## python via python
#if (NOT CMAKE_BUILD_TYPE MATCHES Debug AND SKBUILD)
#    message(STATUS "Skbuild process triggered.")
#
#    set(PYBIND_MAIN python/main.cpp)
#    set(PYBIND_MODULE _nconpp)
#
#    include(FetchContent)
#    FetchContent_Declare(
#            pybind11
#            GIT_REPOSITORY https://github.com/pybind/pybind11
#            GIT_TAG v2.9.1
#    )
#
#    FetchContent_Declare(
#            xtensor-python
#            GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-python
#            GIT_TAG 0.25.3
#    )
#
#    FetchContent_MakeAvailable(pybind11 xtensor-python)
#
#    set(Python_FIND_VIRTUALENV FIRST) # needed for poetry's virtualenv
#    find_package(Python REQUIRED COMPONENTS Interpreter Development NumPy)
#    message(STATUS "Use NumPy include dir: ${Python_NumPy_INCLUDE_DIRS}")
#
#    pybind11_add_module(${PYBIND_MODULE}
#            ${PYBIND_MAIN}
#            ${NCONPP_INCLUDE}
#            ${NCONPP_SRC})
#
#    target_include_directories(${PYBIND_MODULE} PRIVATE
#            ${NCONPP_CORE_DIR}
#            ${NCONPP_INCLUDE_DIR}
#            ${NCONPP_SRC_DIR}
#            ${Python_NumPy_INCLUDE_DIRS})
#
#    target_link_libraries(${PYBIND_MODULE} PRIVATE
#            ${NCONPP_DEPENDENCIES}
#            xtensor-python)
#
#    target_compile_definitions(${PYBIND_MODULE} PRIVATE
#            VERSION_INFO=${VERSION_INFO})
#
#    #message(STATUS "PYTHON_LIBRARY_DIR is ${PYTHON_LIBRARY_DIR}")
#
#    install(TARGETS ${PYBIND_MODULE}
#            COMPONENT python
#            LIBRARY DESTINATION .)
#endif()