# python
project(nconpp_python LANGUAGES CXX)

set(Python_FIND_VIRTUALENV FIRST)
set(Python_ROOT_DIR $ENV{PYTHON_VENV})
message(STATUS "Use Python venv: $ENV{PYTHON_VENV}")
# the format Python_EXECUTABLE is correct
# otherwise some packages won't find the interpreter
message(STATUS "Use Python executable: ${Python_EXECUTABLE}")

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import numpy; print(numpy.get_include())"
    OUTPUT_VARIABLE Python_NumPy_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

message(STATUS "Use Python_NumPy_INCLUDE_DIR: ${Python_NumPy_INCLUDE_DIR}")

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_path('include'))"
    OUTPUT_VARIABLE Python_INCLUDE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

# set(Python_INCLUDE_DIR $ENV{PYTHON_VENV}/include)
message(STATUS "Use Python_INCLUDE_DIR: ${Python_INCLUDE_DIR}")

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))"
    OUTPUT_VARIABLE Python_LIBRARY
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # set(Python_LIBRARY $ENV{PYTHON_VENV}/lib)
message(STATUS "Use Python_LIBRARY: ${Python_LIBRARY}")

execute_process(
    COMMAND ${Python_EXECUTABLE} -c "import pybind11; print(pybind11.get_cmake_dir())"
    OUTPUT_VARIABLE pybind11_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

message(STATUS "Use pybind11_DIR: ${pybind11_DIR}")

find_package(Python COMPONENTS Interpreter Development.Module NumPy REQUIRED)

# pybind11
find_package(pybind11 CONFIG)
set(PYBIND11_NEWPYTHON ON)
set(PYBIND11_MAIN ${NCONPP_PY_DIR}/src/nconpp.cpp)
set(PYBIND11_MODULE _nconpp)

pybind11_add_module(${PYBIND11_MODULE}
    ${PYBIND11_MAIN}
    ${NCONPP_SRC}
    )

# xtensor-python
set(XTENSOR_VERSION ${xtensor_VERSION})
set(XTENSOR_VERSION_MAJOR ${xtensor_VERSION_MAJOR})
set(XTENSOR_VERSION_MINOR ${xtensor_VERSION_MINOR})
set(XTENSOR_VERSION_PATCH ${xtensor_VERSION_PATCH})
find_package(xtensor REQUIRED)
add_subdirectory(${NCONPP_PY_DIR}/extern/xtensor-python)

target_include_directories(${PYBIND11_MODULE} PRIVATE
    ${NCONPP_SRC_DIR}
    ${NCONPP_PY_DIR}/extern/)

target_link_libraries(${PYBIND11_MODULE} PRIVATE
    ${NCONPP_DEPENDENCIES}
    pybind11::module
    xtensor-python)

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

if(SKBUILD)
    message(STATUS "Skbuild process triggered.")

    message(STATUS "Use NumPy include dir: ${Python_NumPy_INCLUDE_DIRS}")

    if(WIN32)
        set(VCPKG_INSTALLED_DIR_DLL_DIR ${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/)

        if(EXISTS ${VCPKG_INSTALLED_DIR_DLL_DIR})
            message(STATUS "Found DLL directory: " ${VCPKG_INSTALLED_DIR_DLL_DIR})
        else()
            message(FATAL_ERROR "Could not find DLL directory.")
        endif()

        file(WRITE ${NCONPP_PY_DIR}/src/__init__.py
"import os, sys

if (sys.platform == 'win32'):
    os.add_dll_directory(\"${VCPKG_INSTALLED_DIR_DLL_DIR}\")

from ._nconpp import Graph, LatticeGraph, TensorNetwork

__all__ = (\"Graph\", \"LatticeGraph\", \"TensorNetwork\")
__version__ = \"0.1.0\"")
    endif()

    if(UNIX)
        file(WRITE ${NCONPP_PY_DIR}/src/__init__.py
"from ._nconpp import Graph, LatticeGraph, TensorNetwork

__all__ = (\"Graph\", \"LatticeGraph\", \"TensorNetwork\")
__version__ = \"0.1.0\"")
    endif()

    target_include_directories(${PYBIND11_MODULE} PRIVATE
        ${Python_NumPy_INCLUDE_DIRS}
        ${BLAS_INCLUDE_DIR})

    target_link_libraries(${PYBIND11_MODULE} PRIVATE
        Python::NumPy)

    target_compile_definitions(${PYBIND11_MODULE} PRIVATE
        VERSION_INFO=${VERSION_INFO})

    install(FILES ${NCONPP_PY_DIR}/src/__init__.py
        DESTINATION .)
    install(TARGETS ${PYBIND11_MODULE}
        LIBRARY DESTINATION .)
endif()
