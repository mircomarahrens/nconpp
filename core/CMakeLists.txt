message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

set(NCONPP_MAIN_TARGET nconpp)

set(NCONPP_MAIN
    ${NCONPP_DIR}/NetworkContractor.h
    ${NCONPP_DIR}/NetworkContractor.cpp)

set(NCONPP_INCLUDE
    include/Graph.h
    include/Network.h)
string(REPLACE "include/" "${NCONPP_DIR}/include/"
    NCONPP_INCLUDE "${NCONPP_INCLUDE}")

set(NCONPP_SRC
    src/Graph.cpp
    src/Network.cpp)
string(REPLACE "src/" "${NCONPP_DIR}/src/"
    NCONPP_SRC "${NCONPP_SRC}")

set(NCONPP_UTILS
    utils/Container.h
    utils/ErrorMessages.h
    utils/Tensor.h)
string(REPLACE "utils/" "${NCONPP_DIR}/utils/"
    NCONPP_INCLUDE "${NCONPP_UTILS}")

add_library(${NCONPP_MAIN_TARGET} STATIC
    ${NCONPP_MAIN}
    ${NCONPP_INCLUDE}
    ${NCONPP_SRC}
    ${NCONPP_UTILS})

target_include_directories(${NCONPP_MAIN_TARGET} PRIVATE
    ${NCONPP_DIR}
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_SRC_DIR}
    ${NCONPP_UTILS_DIR})
    
target_link_libraries(${NCONPP_MAIN_TARGET} PRIVATE
    ${NCONPP_LIBRARIES})
