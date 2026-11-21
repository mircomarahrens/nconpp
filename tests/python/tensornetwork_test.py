import numpy as np

from nconpp import TensorNetwork


class TestTensorNetwork:
    def test_tensor_network(self):
        tensorList = [
            (np.random.randn(60) + 1j * np.random.randn(60)).reshape(3, 4, 5),
            (np.random.randn(3780) + 1j * np.random.randn(3780)).reshape(5, 3, 6, 7, 6),
            (np.random.randn(14) + 1j * np.random.randn(14)).reshape(7, 2),
            (np.random.randn(8) + 1j * np.random.randn(8)).reshape(8),
            (np.random.randn(72) + 1j * np.random.randn(72)).reshape(8, 9),
            (np.random.randn(81) + 1j * np.random.randn(81)).reshape(9, 9),
        ]

        legsList = [[3, -2, 2], [2, 3, 1, 4, 1], [4, -1], [5], [5, -3], [6, 6]]

        tn = TensorNetwork(tensorList, legsList)

        assert tn.num_tensors == 6
        tn.contract()
        assert tn.num_tensors == 3
        tn.connect()
        assert tn.num_tensors == 1

    def test_multi_index_contract(self):
        rng = np.random.default_rng(42)
        tensor_list = [
            rng.normal(size=(3, 4, 5)) + 1j * rng.normal(size=(3, 4, 5)),
            rng.normal(size=(5, 3, 6, 7, 6)) + 1j * rng.normal(size=(5, 3, 6, 7, 6)),
            rng.normal(size=(7, 2)) + 1j * rng.normal(size=(7, 2)),
        ]
        legs_list = [[3, -2, 2], [2, 3, 1, 4, 1], [4, -1]]

        tn_single = TensorNetwork([t.copy() for t in tensor_list], legs_list)
        tn_multi = TensorNetwork([t.copy() for t in tensor_list], legs_list)

        tn_single.contract(multi_index=False)
        tn_multi.contract(multi_index=True)

        assert tn_multi.num_tensors == tn_single.num_tensors == 1
        assert tn_multi.get_legs() == tn_single.get_legs()
        assert np.allclose(tn_multi.vertices[0].tensor, tn_single.vertices[0].tensor)

    def test_multi_index_contract_full_scalar(self):
        rng = np.random.default_rng(7)
        a = rng.normal(size=(4, 2, 3)) + 1j * rng.normal(size=(4, 2, 3))
        b = rng.normal(size=(3, 4, 2)) + 1j * rng.normal(size=(3, 4, 2))

        tn_single = TensorNetwork([a.copy(), b.copy()], [[3, 1, 2], [2, 3, 1]])
        tn_multi = TensorNetwork([a.copy(), b.copy()], [[3, 1, 2], [2, 3, 1]])

        tn_single.contract(multi_index=False)
        tn_multi.contract(multi_index=True)

        assert tn_multi.num_tensors == 1
        assert tn_multi.vertices[0].tensor.shape == ()
        assert np.allclose(tn_multi.vertices[0].tensor, tn_single.vertices[0].tensor)
        assert np.allclose(
            tn_multi.vertices[0].tensor,
            np.tensordot(a, b, axes=([1, 2, 0], [2, 0, 1])),
        )
