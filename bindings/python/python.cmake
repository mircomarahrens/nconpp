# Python bindings configuration

# Python find package
find_package(Python 3.11 COMPONENTS Interpreter Development.Module NumPy REQUIRED)

# Locate pybind11 CMake config from the Python environment
if(NOT pybind11_DIR)
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE pybind11_DIR
        RESULT_VARIABLE pybind11_FIND_RESULT
    )
    if(NOT pybind11_FIND_RESULT EQUAL 0)
        message(FATAL_ERROR "Could not find pybind11 cmake directory using ${Python_EXECUTABLE} -m pybind11 --cmakedir")
    endif()
endif()

# pybind11
find_package(pybind11 CONFIG REQUIRED)

# nconpp Python module
set(PYBIND11_MODULE _nconpp)

pybind11_add_module(${PYBIND11_MODULE}
    MODULE
    ${NCONPP_BINDINGS_DIR}/python/bindings.cpp
    ${NCONPP_CPP_HEADERS}
    )

# xtensor-python
set(XTENSOR_VERSION ${xtensor_VERSION})
set(XTENSOR_VERSION_MAJOR ${xtensor_VERSION_MAJOR})
set(XTENSOR_VERSION_MINOR ${xtensor_VERSION_MINOR})
set(XTENSOR_VERSION_PATCH ${xtensor_VERSION_PATCH})
find_package(xtensor REQUIRED)
add_subdirectory(${NCONPP_EXTERN_DIR}/xtensor-python)

target_include_directories(${PYBIND11_MODULE} PRIVATE
    ${NCONPP_INCLUDE_DIR}
    ${NCONPP_INCLUDE_DIR}/nconpp
    ${NCONPP_EXTERN_DIR}
    ${Python_NumPy_INCLUDE_DIRS}
    ${BLAS_INCLUDE_DIR}
    )

target_link_libraries(${PYBIND11_MODULE} PRIVATE
    ${NCONPP_DEPENDENCIES}
    pybind11::module
    xtensor-python
    Python::NumPy
    )

if(SKBUILD)
    message(STATUS "Skbuild process triggered.")
    message(STATUS "Use NumPy include dir: ${Python_NumPy_INCLUDE_DIRS}")

    target_compile_definitions(${PYBIND11_MODULE} PRIVATE
        VERSION_INFO=${VERSION_INFO})

    install(TARGETS ${PYBIND11_MODULE}
        LIBRARY DESTINATION nconpp
        ARCHIVE DESTINATION nconpp
        RUNTIME DESTINATION nconpp)
endif()
