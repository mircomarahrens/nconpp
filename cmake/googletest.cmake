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
    src/
    ${BLAS_INCLUDE_DIR})

target_link_libraries(nconpp_tests_run
    ${CMAKE_THREAD_LIBS_INIT}
    ${NCONPP_DEPENDENCIES}
    gmock_main
    gtest_main)

# include(CTest)
include(GoogleTest)
gtest_discover_tests(nconpp_tests_run)