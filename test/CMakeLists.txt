message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

set(NCONPP_TEST_TARGET "nconpp_test")

set(NCONPP_TEST_MAIN
    nconpp_test.cpp)

set(NCONPP_TEST_UT
    ${NCONPP_UT_DIR}/NetworkTest.cpp
    ${NCONPP_UT_DIR}/NconppTest.cpp
    ${NCONPP_UT_DIR}/GraphTest.cpp)

add_executable(${NCONPP_TEST_TARGET}
    ${NCONPP_TEST_MAIN}
    ${NCONPP_TEST_UT})

target_include_directories(${NCONPP_TEST_TARGET} PRIVATE
    ${NCONPP_CORE_DIR}
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_SRC_DIR}
    ${NCONPP_UTILS_DIR})

target_link_libraries(${NCONPP_TEST_TARGET}
    ${CMAKE_THREAD_LIBS_INIT}
    ${NCONPP_DEPENDENCIES}
    gtest_main
    nconpp)

include(GoogleTest)
gtest_discover_tests(${NCONPP_TEST_TARGET})
