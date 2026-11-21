import nconpp
from nconpp import algorithms, core
from nconpp.core import Graph, LatticeGraph, TensorNetwork


class TestPackageStructure:
    def test_top_level_exports(self):
        assert hasattr(nconpp, "Graph")
        assert hasattr(nconpp, "LatticeGraph")
        assert hasattr(nconpp, "TensorNetwork")
        assert hasattr(nconpp, "core")
        assert hasattr(nconpp, "algorithms")

    def test_core_exports(self):
        assert core.Graph is nconpp.Graph
        assert core.LatticeGraph is nconpp.LatticeGraph
        assert core.TensorNetwork is nconpp.TensorNetwork
        assert hasattr(core, "EdgeProperties")
        assert hasattr(core, "VertexProperties")

    def test_core_instance(self):
        g = Graph(2)
        assert g.num_vertices == 2
        assert isinstance(g, core.Graph)

    def test_algorithms_subpackage(self):
        assert hasattr(algorithms, "__all__")
