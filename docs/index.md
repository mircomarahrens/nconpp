# nconpp

A C++ library for tensor network operations with Python bindings.

`nconpp` facilitates efficient contraction and decomposition of tensor networks. It is inspired by the MATLAB library NCON ([arXiv:1402.0939](https://arxiv.org/abs/1402.0939)) and extensively leverages graph algorithms and linear algebra.

---

## Features

- **Graph Abstractions**: Underpins tensor network topology with adjacency structures (`Graph`, `LatticeGraph`).
- **Tensor Network Contractions**: Fast contraction sequences with validation and boundary conditions (`TensorNetwork`).
- **Modern C++20 & Linear Algebra**: Native integration with `xtensor` and BLAS/LAPACK.
- **Python Bindings**: Seamless `numpy` and `pybind11` interoperability with complete type annotations.

---

## Quick Navigation

<div class="grid cards" markdown>

-   :material-language-python: __Python API__

    ---

    Explore classes and methods for Python users: `nconpp.Graph`, `nconpp.LatticeGraph`, and `nconpp.TensorNetwork`.

    [:octicons-arrow-right-24: Python Documentation](python/index.md)

-   :material-language-cpp: __C++ API__

    ---

    Explore C++ classes, templates, and functions in the `npp` namespace.

    [:octicons-arrow-right-24: C++ Documentation](cpp/index.md)

</div>
