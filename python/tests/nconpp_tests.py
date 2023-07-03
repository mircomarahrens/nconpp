import numpy as np
from nconpp import Graph, TensorNetwork

class TestGraph:
    def test_finite_graph(self):
        g = Graph(10)
        assert g.num_vertices == 10

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
