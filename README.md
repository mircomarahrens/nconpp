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
poetry install
poetry env activate
```

- Multi-dimensional arrays with broadcasting and lazy computing: [Xtensor](https://github.com/xtensor-stack/xtensor)

```bash
git clone --branch "0.27.0" --depth 1 --single-branch https://github.com/xtensor-stack/xtensor-python.git python/extern/xtensor-python
# the CMakeLists is outdated and needed an update 
mv python/extern/CMakeLists_xtensor-python.txt python/extern/xtensor-python/CMakeLists.txt
```

- custom xtensor-python; needs an update by time

```bash
git clone git@github.com:mircomarahrens/xtensor-python.git python/extern/xtensor-python
```

## Build

### Debug

```bash
cmake --preset Debug
```

### Python

```bash
cmake --preset Python
pip install .
```

## Ideas

* Text-based user interface; inspiration: <https://www.tensortrace.com/>
* Add mechanism for empty initilaization of class TensorNetwork with CRUD-like behavior
* Add tests for python via pytest or similar
