message(STATUS "Processing directory " ${CMAKE_CURRENT_SOURCE_DIR})

project(nconpp_python)

set(PYBIND_MAIN      "src/main.cpp")
set(PYBIND_MODULE    "_nconpp")
set(PYBIND_INTERFACE "_nconpp_interface")
set(PYBIND_INIT      "src/nconpp/__init__.py")

# get headers in xtensor-python/include
include(FetchContent)
FetchContent_Declare(xtensor-python
    GIT_REPOSITORY https://github.com/xtensor-stack/xtensor-python
    GIT_TAG "0.26.0"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND "")

FetchContent_GetProperties(xtensor-python)
if(NOT xtensor-python_POPULATED)
  FetchContent_Populate(xtensor-python)
endif()

add_library(xtensor-python INTERFACE)
target_include_directories(xtensor-python INTERFACE 
    ${xtensor-python_SOURCE_DIR}/include)

add_library(${PYBIND_INTERFACE} INTERFACE)

target_include_directories(${PYBIND_INTERFACE} INTERFACE 
    ${NCONPP_CORE_DIR}
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_SRC_DIR}
    ${NCONPP_UTILS_DIR})

target_link_libraries(${PYBIND_INTERFACE} INTERFACE
    nconpp
    xtensor-python)

target_compile_definitions(${PYBIND_INTERFACE} INTERFACE
    VERSION_INFO=${VERSION_INFO})

if (SKBUILD)
    message(STATUS "Skbuild process triggered.")
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development NumPy)
    message(STATUS "Use numpy include dir: ${Python3_NumPy_INCLUDE_DIRS}")

    # Scikit-Build does not add your site-packages to the search path
    # automatically, so we need to add it _or_ the pybind11 specific directory
    # here.
    execute_process(
        COMMAND "${Python3_EXECUTABLE}" -c "import pybind11; print(pybind11.get_cmake_dir())"
        OUTPUT_VARIABLE _tmp_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT
    )
    message(STATUS "Use CMake dir: ${_tmp_dir}")
    list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")

    find_package(pybind11 CONFIG REQUIRED)

    pybind11_add_module(${PYBIND_MODULE} MODULE
        ${PYBIND_MAIN})

    target_include_directories(${PYBIND_MODULE} PRIVATE
        ${Python3_NumPy_INCLUDE_DIRS})

    target_link_libraries(${PYBIND_MODULE} PRIVATE
        ${PYBIND_INTERFACE})

    install(TARGETS ${PYBIND_MODULE} LIBRARY
    COMPONENT python 
    DESTINATION .)

    install(FILES ${PYBIND_INIT}
    COMPONENT python
    DESTINATION .)
endif()