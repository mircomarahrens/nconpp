message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

set(NCONPP_LIBRARY_TARGET "nconpp")

set(NCONPP_MAIN
    ${NCONPP_CORE_DIR}/NetworkContractor.h
    ${NCONPP_CORE_DIR}/NetworkContractor.cpp)

set(NCONPP_INCLUDE
    ${NCONPP_INCLUDE_DIR}/Graph.h
    ${NCONPP_INCLUDE_DIR}/Network.h)

set(NCONPP_SRC
    ${NCONPP_SRC_DIR}/Graph.cpp
    ${NCONPP_SRC_DIR}/Network.cpp)

set(NCONPP_UTILS
    ${NCONPP_UTILS_DIR}/Container.h
    ${NCONPP_UTILS_DIR}/ErrorMessages.h
    ${NCONPP_UTILS_DIR}/Tensor.h)

add_library(${NCONPP_LIBRARY_TARGET} STATIC
    ${NCONPP_MAIN}
    ${NCONPP_INCLUDE}
    ${NCONPP_SRC}
    ${NCONPP_UTILS})

target_include_directories(${NCONPP_LIBRARY_TARGET} PRIVATE
    ${NCONPP_CORE_DIR}
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_SRC_DIR}
    ${NCONPP_UTILS_DIR})

target_link_libraries(${NCONPP_LIBRARY_TARGET}
    ${NCONPP_DEPENDENCIES})
