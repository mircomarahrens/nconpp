message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

set(NCONPP_LIBRARY_TARGET "nconpp")

set(NCONPP_MAIN
        ${NCONPP_CORE_DIR}/NetworkContractor.h
        ${NCONPP_CORE_DIR}/NetworkContractor.cpp)

set(NCONPP_INCLUDE
        ${NCONPP_INCLUDE_DIR}/Algorithms.h
        ${NCONPP_INCLUDE_DIR}/Container.h
        ${NCONPP_INCLUDE_DIR}/ErrorMessages.h
        ${NCONPP_INCLUDE_DIR}/Graph.h
        ${NCONPP_INCLUDE_DIR}/Network.h
        ${NCONPP_INCLUDE_DIR}/Tensor.h)

set(NCONPP_SRC
        ${NCONPP_SRC_DIR}/Algorithms.cpp
        ${NCONPP_SRC_DIR}/Graph.cpp
        ${NCONPP_SRC_DIR}/Network.cpp)

add_library(${NCONPP_LIBRARY_TARGET} SHARED
        ${NCONPP_MAIN}
        ${NCONPP_INCLUDE}
        ${NCONPP_SRC})

target_include_directories(${NCONPP_LIBRARY_TARGET} PRIVATE
        ${NCONPP_CORE_DIR}
        ${NCONPP_INCLUDE_DIR}
        ${NCONPP_SRC_DIR})

target_link_libraries(${NCONPP_LIBRARY_TARGET}
        ${NCONPP_DEPENDENCIES})
