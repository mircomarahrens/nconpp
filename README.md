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
git clone https://github.com/microsoft/vcpkg utils/vcpkg
```

- C++ build system: [CMake](https://cmake.org/)
- Python binding: [pybind11](https://pybind11.readthedocs.io/)
- Python package manager: [Poetry](https://python-poetry.org/)

```bash
poetry shell
poetry install
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
