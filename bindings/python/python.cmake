# Python bindings configuration

find_package(Python COMPONENTS Interpreter Development.Module NumPy REQUIRED)

# pybind11: Try standard CMake config first, then fallback to python module directory
find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
    execute_process(
        COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE pybind11_DIR
    )
    find_package(pybind11 CONFIG REQUIRED)
endif()

set(PYBIND11_MODULE _nconpp)

pybind11_add_module(${PYBIND11_MODULE}
    MODULE
    NO_EXTRAS
    ${CMAKE_CURRENT_SOURCE_DIR}/bindings/python/bindings.cpp
)

target_include_directories(${PYBIND11_MODULE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/xtensor-python/include
)

target_link_libraries(${PYBIND11_MODULE} PRIVATE
    nconpp
    pybind11::module
    Python::NumPy
)

if(SKBUILD)
    target_compile_definitions(${PYBIND11_MODULE} PRIVATE
        VERSION_INFO=${VERSION_INFO}
    )

    install(TARGETS ${PYBIND11_MODULE}
        LIBRARY DESTINATION nconpp
        ARCHIVE DESTINATION nconpp
        RUNTIME DESTINATION nconpp
    )
endif()
