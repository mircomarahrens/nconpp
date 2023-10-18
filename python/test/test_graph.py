import numpy as np
from nconpp import Graph

class TestGraph:
    def test_finite_graph(self):
        g = Graph(5)
        assert g.num_vertices == 5
        assert g.get_vertices() == {0,1,2,3,4}
        assert type(g.vertices[0].edge_indices) is set
        g.remove_vertex(3)
        assert g.get_vertices() == {0,1,2,4}
        g.add_vertex(5)
        assert g.get_vertices() == {0,1,2,4,5}
        g.add_edge(0,1,2)
        assert g.get_edges() == {0}
        assert g.num_edges == 1
        assert g.edges[0].src == 1
        assert g.edges[0].dest == 2
        g.remove_edge(0)
        assert g.num_edges == 0
        assert not g.get_edges()
