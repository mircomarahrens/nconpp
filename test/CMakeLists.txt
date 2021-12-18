message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

set(NCONPP_TEST_TARGET nconpp_test)

set(NCONPP_TEST_MAIN
    nconpp_test.cpp)

set(NCONPP_TEST_UT
    ut/networkTest.cpp
    ut/nconppTest.cpp)

add_executable(${NCONPP_TEST_TARGET}
    ${NCONPP_TEST_MAIN}
    ${NCONPP_TEST_UT})

target_include_directories(${NCONPP_TEST_TARGET} PRIVATE
    ${NCONPP_DIR}
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_SRC_DIR}
    ${NCONPP_UTILS_DIR})

target_link_libraries(${NCONPP_TEST_TARGET}
    ${GTEST_BOTH_LIBRARIES}
    ${NCONPP_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    nconpp)

add_test(NAME allTests COMMAND ${NCONPP_TEST_TARGET})
