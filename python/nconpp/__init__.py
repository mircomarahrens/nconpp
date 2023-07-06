import os, sys

if (sys.platform == 'win32'):
    os.add_dll_directory("C:/Repositories/nconpp/out/build/python/vcpkg_installed/x64-windows/bin/")

from ._nconpp import TensorNetwork, Graph, VertexProperties, EdgeProperties

__all__ = ("TensorNetwork", "Graph", "VertexProperties", "EdgeProperties")