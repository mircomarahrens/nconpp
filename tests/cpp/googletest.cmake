include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.15.2
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(nconpp_tests_run
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/nconpp_tests_run.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/GraphTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/LatticeGraphTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/LinAlgTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/TensorTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/TensorNetworkTest.cpp
)

target_link_libraries(nconpp_tests_run PRIVATE
    nconpp
    gmock_main
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(nconpp_tests_run)
