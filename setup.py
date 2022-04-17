import os

from skbuild import setup
from setuptools import find_packages

# cmake specific
__cmake_args__= []
# For our 3rd party library handling we used the C++ package manager vcpkg
# (see https://github.com/microsoft/vcpkg for details). Vcpkg offers a cmake
# toolchain file which we have to reveal to the cmake installer.
__toolchain__ = "/home/mircomarahrens/vcpkg/scripts/buildsystems/vcpkg.cmake"
#__toolchain__ = os.getenv("CMAKE_TOOLCHAIN_FILE")
__cmake_args__.append("-DCMAKE_TOOLCHAIN_FILE={}".format(__toolchain__))

setup(
    name="nconpp",
    version="0.1",
    description="Library for Tensor Network contraction.",
    author="Mirco Marahrens",
    author_email="mirco.marahrens@googlemail.com",
    url="https://github.com/mircomarahrens/nconpp-python",
    packages=find_packages(where="pybind"),
    package_dir={"": "pybind"},
    cmake_args=__cmake_args__,
    python_requires=">=3.6",
)
