message(STATUS "GoogleTest process triggered.")
project(nconpp_tests)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.15.2
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(nconpp_tests_run
    ${NCONPP_CPP_TEST_DIR}/nconpp_tests_run.cpp
    ${NCONPP_CPP_TEST_DIR}/GraphTest.cpp
    ${NCONPP_CPP_TEST_DIR}/LatticeGraphTest.cpp
    ${NCONPP_CPP_TEST_DIR}/TensorTest.cpp
    ${NCONPP_CPP_TEST_DIR}/TensorNetworkTest.cpp)

target_include_directories(nconpp_tests_run PUBLIC
    ${NCONPP_CPP_SRC_DIR}
    ${BLAS_INCLUDE_DIR})

target_link_libraries(nconpp_tests_run
    ${CMAKE_THREAD_LIBS_INIT}
    ${NCONPP_DEPENDENCIES}
    gmock_main
    gtest_main)

# include(CTest)
include(GoogleTest)
gtest_discover_tests(nconpp_tests_run)
