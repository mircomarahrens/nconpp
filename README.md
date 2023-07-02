# Efficient tensor network contraction and decomposition with ``nconpp``

This module shall help to perform efficient contractions and decomposition of tensor networks. The module is
inspired by the matlab library NCON (<https://arxiv.org/abs/1402.0939>) and leverages strongly on graph algorithms.

## Getting Started

* cmake
* vcpkg
* presets

## Install as Python module

Execute the following commands if you wish to install ``nconpp`` in a virtual environment.

```bash
git clone https://github.com/mircomarahrens/nconpp.git
cd nconpp
```

* Linux

```bash
python -m venv venv # or python3
source ./venv/bin/activate
python -m pip install .
```

* Windows with `pylauncher`

```bash
py -m venv venv
.\venv\Scripts\activate
pip install .
```

## References

* [BGL (Boost Graph Library)](https://www.boost.org/doc/libs/1_80_0/libs/graph/doc/index.html)
* [pybind11](https://pybind11.readthedocs.io/)
* [vcpkg](https://vcpkg.io/en/)
* [scikit-build-core](https://scikit-build-core.readthedocs.io/)
* [xtensor](https://xtensor.readthedocs.io/)

## Licensing

...

## Ideas

* Text-based user interface; inspiration: <https://www.tensortrace.com/>
* Add mechanism for empty initilaization of class TensorNetwork with CRUD-like behavior
* Add tests for python via pytest or similar
