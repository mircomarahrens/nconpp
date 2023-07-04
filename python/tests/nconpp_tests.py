import numpy as np
from nconpp import Graph, TensorNetwork

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

class TestTensorNetwork:
    def test_tensor_network(self):
        tensorList = [
            (np.random.randn(60)+1j*np.random.randn(60)).reshape(3,4,5),
            (np.random.randn(3780)+1j*np.random.randn(3780)).reshape(5, 3, 6, 7, 6),
            (np.random.randn(14)+1j*np.random.randn(14)).reshape(7,2),
            (np.random.randn(8)+1j*np.random.randn(8)).reshape(8),
            (np.random.randn(72)+1j*np.random.randn(72)).reshape(8,9),
            (np.random.randn(81)+1j*np.random.randn(81)).reshape(9,9)
        ]

        legsList = [
            [3, -2, 2],
            [2, 3, 1, 4, 1],
            [4, -1],
            [5],
            [5, -3],
            [6, 6]
        ]
        
        tn = TensorNetwork(tensorList, legsList)
        
        assert tn.num_tensors == 6
        tn.contract()
        assert tn.num_tensors == 3
        tn.connect()
        assert tn.num_tensors == 1
