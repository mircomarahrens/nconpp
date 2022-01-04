# Efficient network contraction with "nconpp"

Writing efficient algorithms for the contraction of networks can be exhausting. Performing repetitive tasks which only differ slightly can lead to errors and mistakes. At worst the programmer quits his idea. This module shall help to perform efficient contractions of networks with low effort based on graph algorithms.

## Breadth first search and Depth first search

## Literature

Graph algorithms in the Language of Linear Algebra


## ToDo

- [x] rename Set class to Container
- [ ] index position by iterator, not by index would be faster
- [ ] checkout Travis CI
- [ ] resources: discarded weight, contraction error (breadth vs depth first vs combination?)

## Docker image

```bash
docker build --target build -t nconpp-builder .
```

# CMake Toolchain

## Windows + WSL2

```bash
sudo apt install build-essential g++ gdb make ninja-build rsync zip
```

## Python

From root folder `nconpp`.

* Linux

```bash
python -m venv nconpp-venv # or python3 ...
source ./nconpp-venv/bin/activate
python -m pip install py/.
```

* Windows

```bash
py -m venv nconpp-venv
.\nconpp-venv\Scripts\activate
pip install py\.
```
