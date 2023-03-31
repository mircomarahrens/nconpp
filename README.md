# Efficient tensor network contraction and decomposition with ``nconpp``

This module shall help to perform efficient contractions and decomposition of tensor networks. The module is
inspired by the matlab library NCON (<https://arxiv.org/abs/1402.0939>) and leverages strongly on mGraph algorithms.

## Install the Python module

Execute the following commands if you wish to install ``nconpp`` in a virtual environment.

* Linux

```bash
python -m venv venv # or python3
source ./venv/bin/activate
python -m pip install $PATH_TO_NCONPP # $PATH_TO_NCONPP = ., if in root dir of nconpp
```

* Windows with `pylauncher`

```bash
py -m venv venv
.\venv\Scripts\activate
pip install $PATH_TO_NCONPP # $PATH_TO_NCONPP = ., if in root dir of nconpp
```
