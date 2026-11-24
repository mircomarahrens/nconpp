include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG v1.9.2
)
FetchContent_MakeAvailable(googlebenchmark)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(benchmark PRIVATE -Wno-unknown-warning-option -Wno-c2y-extensions)
    target_compile_options(benchmark_main PRIVATE -Wno-unknown-warning-option -Wno-c2y-extensions)
endif()

add_executable(nconpp_benchmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/nconpp_benchmarks.cpp
)

target_link_libraries(nconpp_benchmarks PRIVATE
    nconpp
    benchmark::benchmark_main
)

# Always build benchmarks with optimizations enabled, even under Debug presets.
if(NOT MSVC)
    target_compile_options(nconpp_benchmarks PRIVATE -O3)
else()
    target_compile_options(nconpp_benchmarks PRIVATE /O2)
endif()
target_compile_definitions(nconpp_benchmarks PRIVATE NDEBUG)
