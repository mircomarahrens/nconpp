# Efficient tensor network contraction and decomposition with ``nconpp``

This module shall help to perform efficient contractions and decomposition of tensor networks. The module is
inspired by the matlab library NCON (<https://arxiv.org/abs/1402.0939>) and leverages strongly on graph algorithms.

## Local development environment

```bash
git clone https://github.com/mircomarahrens/nconpp.git
cd nconpp
```

### Project dependencies

- C++ Package manager: [vcpkg](https://vcpkg.io/en/)

```bash
git clone --branch "2025.04.09" --single-branch https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh --disableMetrics
./vcpkg integrate install
```

- C++ build system: [CMake](https://cmake.org/)
- Python binding: [pybind11](https://pybind11.readthedocs.io/)
- Python package manager: [Poetry](https://python-poetry.org/)

```bash
uv venv
source .venv/bin/activate
```

- Multi-dimensional arrays with broadcasting and lazy computing: [Xtensor](https://github.com/xtensor-stack/xtensor)

```bash
git clone --branch "0.27.0" --depth 1 --single-branch https://github.com/xtensor-stack/xtensor-python.git python/extern/xtensor-python
# the CMakeLists is outdated and needed an update 
cp python/extern/CMakeLists_xtensor-python.txt python/extern/xtensor-python/CMakeLists.txt
```

## Build

### C++ Debug

```bash
cmake --preset linux-debug
cmake --build build/linux-debug
```

### Python

```bash
cmake --preset Python
uv pip install .
```

## Testing

### C++

```bash
ctest --test-dir build/linux-debug
```

## Development

If you are using Neovim, a language server like clangd and getting messages like "Header file not found", you can symlink the `compile_commands.json` file to the root directory of the project:

```bash
ln -s build/linux-debug/compile_commands.json .
```

or for the Python bindings:

```bash
ln -s build/python/compile_commands.json .
```

## Ideas

- Text-based user interface; inspiration: <https://www.tensortrace.com/>
- Add mechanism for empty initialization of class TensorNetwork with CRUD-like behavior
- Add tests for python via pytest or similar
