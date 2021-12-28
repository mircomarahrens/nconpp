# -*- coding: utf-8 -*-
import sys
from skbuild import setup
from pathlib import Path
from setuptools import find_packages

# general
__name__ = "nconpp"
__version__ = "0.0.1"

# cmake specific
__cmake_args__= []
# For our 3rd party library handling we used the C++ package manager vcpkg 
# (see https://github.com/microsoft/vcpkg for details). Vcpkg offers a cmake
# toolchain file which we have to reveal to the cmake installer.
if sys.platform == "win32":
    __toolchain__ = "C:\\Users\\MircoMarahrens\\source\\repos\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake"
    __cmake_args__.append("-DCMAKE_GENERATOR_PLATFORM={}".format("x64")) # Visual Studio specific
elif sys.platform == "linux":
    __toolchain__ = "/home/mircomarahrens/Git/vcpkg/scripts/buildsystems/vcpkg.cmake"

__cmake_source_dir__ = Path.cwd()

__cmake_args__.append("-DCMAKE_TOOLCHAIN_FILE={}".format(__toolchain__))
__cmake_args__.append("-DVERSION_INFO={}".format(__version__))

setup(
    name=__name__,
    version=__version__,
    author="Mirco Marahrens",
    author_email="mirco.marahrens@googlemail.com",
    description="Library for Tensor Network contraction.",
    packages=find_packages(where = 'python/src'),
    url="https://github.com/mircomarahrens/nconpp",
    package_dir={"" : "py/src"},
    cmake_args=__cmake_args__,
    cmake_source_dir = __cmake_source_dir__,
    cmake_install_dir = "py/src/nconpp",
    include_package_data = True,
)
