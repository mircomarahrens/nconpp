import os

from skbuild import setup
from setuptools import find_packages

# cmake specific
__cmake_args__= []

__toolchain__ = "C:/Repositories/vcpkg/scripts/buildsystems/vcpkg.cmake"
__cmake_args__.append("-DCMAKE_TOOLCHAIN_FILE={}".format(__toolchain__))

#__python_library_dir__ = "/home/mircomarahrens/Repositories/nconpp/nconpp-venv/lib/python3.10/site-packages"
#__cmake_args__.append("-DPYTHON_LIBRARY_DIR={}".format(__python_library_dir__))

setup(
    name="nconpp",
    version="0.1",
    description="Library for Tensor Network contraction.",
    author="Mirco Marahrens",
    author_email="mirco.marahrens@googlemail.com",
    url="https://github.com/mircomarahrens/nconpp",
    packages=find_packages(where="python"),
    package_dir={"": "python"},
    cmake_install_dir="python/nconpp",
    include_package_data=True,
    cmake_args=__cmake_args__,
    python_requires=">=3.6",
)
