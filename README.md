# NCONPP

A C++ library for tensor network contraction and decomposition.

This module facilitates efficient contraction and decomposition of tensor networks. It is inspired by the MATLAB library NCON (<https://arxiv.org/abs/1402.0939>) and extensively leverages graph algorithms and linear algebra.

## Development

```bash
git clone https://github.com/mircomarahrens/nconpp.git
cd nconpp
```

### Dependencies

The following dependencies are required for local development. The instructions are used to set up the development environment on a Linux system. The author has used:

```bash
lsb_release -a
```

```bash
No LSB modules are available.
Distributor ID: Ubuntu
Description:    Ubuntu 24.04.3 LTS
Release:        24.04
Codename:       noble
```

### C++ Development

- C++ Package manager: [vcpkg](https://vcpkg.io/en/)

```bash
git clone --branch "2025.04.09" --single-branch https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh --disableMetrics
./vcpkg integrate install
```

- C++ build system: [CMake](https://cmake.org/)

```bash
cmake --version
```

```bash
cmake version 3.31.6

CMake suite maintained and supported by Kitware (kitware.com/cmake).
```

### Python Development

- Python package manager: [UV](https://docs.astral.sh/uv/)

```bash
uv --version
```

```bash
uv 0.8.2
```

```bash
uv python list
```

```bash
[...]
cpython-3.12.3-linux-x86_64-gnu                   /usr/bin/python3.12
cpython-3.12.3-linux-x86_64-gnu                   /usr/bin/python3 -> python3.12
cpython-3.12.3-linux-x86_64-gnu                   /usr/bin/python -> python3
[...]
```

- Create and activate a virtual environment:

```bash
uv venv
source .venv/bin/activate
```

- Multi-dimensional arrays with broadcasting and lazy computing: [Xtensor](https://github.com/xtensor-stack/xtensor)

```bash
git clone --branch "0.27.0" --depth 1 --single-branch https://github.com/xtensor-stack/xtensor-python.git python/extern/xtensor-python
# the CMakeLists is outdated and needs an update
cp python/extern/CMakeLists_xtensor-python.txt python/extern/xtensor-python/CMakeLists.txt
```

### Configuration

The project uses CMake presets for configuration. The presets are defined in the `CMakePresets.json` file.

#### C++ configuration

```bash
cmake --preset linux-debug
```

#### Python configuration

```bash
cmake --preset python
```

#### IDE

Here are some instructions for setting up the project in different IDEs.

##### Neovim

If you are using Neovim, a language server like clangd and getting messages like "Header file not found", you can symlink the `compile_commands.json` file to the root directory of the project:

```bash
ln -s build/linux-debug/compile_commands.json .
```

or for the Python bindings:

```bash
ln -s build/python/compile_commands.json .
```

## Build

### C++ Debug

```bash
cmake --preset linux-debug
cmake --build build/linux-debug
```

### Python

```bash
cmake --preset python
uv pip install .
```

## Testing

### C++ Testing

```bash
ctest --test-dir build/linux-debug
```

## Ideas

- Text-based user interface; inspiration: <https://www.tensortrace.com/>
- Add mechanism for empty initialization of class TensorNetwork with CRUD-like behavior
- Add tests for python via pytest or similar
