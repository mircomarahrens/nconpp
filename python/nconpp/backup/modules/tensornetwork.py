import numpy as np
from tnb.modules.graph import GRAPH
from tnb.modules.mps import MPS
from tnb.modules.mpo import MPO

# FIXME


class TENSORNETWORK(GRAPH, MPS, MPO):
    """========================================================================
    Class for construction of tensor networks.
    ========================================================================
    Inherit classes: GRAPH, MPS, MPO
    ========================================================================
    Key             |  Default        | Description
    ----------------+-----------------+-------------------------------------
                    |                 |
    ========================================================================
    """

    def __init__(self):
        super.__init__()  # initilize inherit classes
        self.Le_list = None  # list for left environments
        self.Re_list = None  # list for right environments

    def init_environments(self):
        """Initialize left and right environments as unities."""
        W_list = self.W_list
        M_list = self.M_list
        L = len(W_list)
        self.Le_list = []
        self.Re_list = []
        Le = np.zeros((1, self.wdim, 1), dtype=complex)
        Le[0, 0, 0] = 1.0
        Re = np.zeros((1, self.wdim, 1), dtype=complex)
        Re[0, self.wdim - 1, 0] = 1.0
        for i in range(L):
            Le = np.tensordot(Le, M_list[i], axes=(0, 0))
            Le = np.tensordot(Le, W_list[i], axes=([0, 2], [0, 3]))
            Le = np.tensordot(Le, np.conj(M_list[i]), axes=([0, 3], [0, 1]))
            Re = np.tensordot(Re, M_list[-i - 1], axes=(0, 2))
            Re = np.tensordot(Re, W_list[-i - 1], axes=([0, 3], [1, 3]))
            Re = np.tensordot(Re, np.conj(M_list[-i - 1]), axes=([1, 3], [2, 1]))
            self.Le_list.append(Le)
            self.Re_list = [Re] + self.Re_list
        return (self.Le_list, self.Re_list)

    def contract(self, index=None, mode=None):
        return
