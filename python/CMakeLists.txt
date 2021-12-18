message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

project(nconpp_python)

set(PYBIND_MAIN     "src/main.cpp")
set(PYBIND_MODULE   "_nconpp")
set(PYBIND_INIT     "src/nconpp/__init__.py")

find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(${PYBIND_MODULE} MODULE
    ${PYBIND_MAIN})

# get xtensor-python
include(ExternalProject)
find_package(Git REQUIRED)

set(EXTERNAL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

ExternalProject_Add(xtensor-python
    PREFIX ${EXTERNAL_SOURCE_DIR}
    GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-python
    TIMEOUT 10
    UPDATE_COMMAND ${GIT_EXECUTABLE} pull
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
)

target_include_directories(${PYBIND_MODULE} PRIVATE 
    ${NCONPP_DIR}
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_SRC_DIR}
    ${NCONPP_UTILS_DIR}
    ${EXTERNAL_SOURCE_DIR}/src/xtensor-python/include)

target_link_libraries(${PYBIND_MODULE} PRIVATE
    ${NCONPP_LIBRARIES}
    ${NCONPP_MAIN}
    ${NCONPP_INCLUDE}
    ${NCONPP_SRC}
    ${NCONPP_UTILS})

target_compile_definitions(${PYBIND_MODULE} PRIVATE
    VERSION_INFO=${VERSION_INFO})

if(SKBUILD)
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)
    message(STATUS "Found Python: ${PYTHON_EXECUTABLE}")

    # Scikit-Build does not add your site-packages to the search path
    # automatically, so we need to add it _or_ the pybind11 specific directory
    # here.
    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE _tmp_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT
    )
    message(STATUS "Use CMake dir: ${_tmp_dir}")
    list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")

    # Cmake's find_package was not able to find the NumPy include dir.
    # Therefore we will add it manually.
    execute_process(
        COMMAND "${PYTHON_EXECUTABLE}" -c "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT
    )
    message(STATUS "Use numpy include dir: ${NUMPY_INCLUDE_DIR}")
    list(APPEND CMAKE_PREFIX_PATH "${NUMPY_INCLUDE_DIR}")

    target_include_directories(${PYBIND_MODULE} PRIVATE
        ${NUMPY_INCLUDE_DIR})

    install(TARGETS ${PYBIND_MODULE} LIBRARY
    COMPONENT python 
    DESTINATION .)

    install(FILES ${PYBIND_INIT}
    COMPONENT python
    DESTINATION .)
endif()
