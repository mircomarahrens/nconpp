#!/usr/bin/python3

import numpy as np

from python.nconpp.latticegraph import LatticeGraph
from mps import Mps
from mpo import Mpo


class TensorNetwork(LatticeGraph, Mps, Mpo):
    """Derived class for construction of tensor networks based on matrix
    product states, -operators and a lattice graph.

    A Tensor Network TN=(G,M) can be derived from a Graph G and a set of tensors
    M. The graph G=(V,E) consist of a set of vertices V and edges E which are
    describing the algebraic geometry of the tensor network.

    The directions in a tensor network, especially left "<-" and right "->",
    should always be seen in perspective from a bond. The naming left and right
    enviroments is given through the origin of tensor networks from one
    dimensional chains, where we only have those two directions:

    1D:
      tensor network:
           ->  A  ->  A  ->  A  -> A -> l <-  B  <-  B  <-  B  <-
      graph:
      (i-4)--(i-3)--(i-2)--(i-1)--(i)--(i)--(i+1)--(i+2)--(i+3)--(i+4)
      +--> x

    2D:
        # TODO

    Args:
        local_dimension (int): the dimension of the local Hilbert space.

    Attributes:
        A_list list(np.ndarray): left-canonical "<-" matrix product states
        B_list list(np.ndarray): right-canonical "->" matrix product states
        s_list list(np.ndarray): list of Schmidt values
        M_list list(np.ndarray): list for initial random matrix product state
            and storage container for arbitrary matrix product state, e.g. for
            canonical (aka G. Vidal) matrix product state
        t_list list(np.ndarray): list for eigenvalues of the transfer matrix
        W_list list(np.ndarray): list for matrix product operators
        Le_list list(np.ndarray): list for left "<-" bond environments
        Re_list list(np.ndarray): list for right "->" bond environments

    Methods:
        init_mps_random():
            initilize a random matrix product state
        init_mpo():
            ...
        init_bond_environments():
            initilize the bond environments based on the initilized mps and mpo.

    """

    def __init__(self):
        super.__init__()
        self.A_list = list()
        self.B_list = list()
        self.s_list = list()
        self.M_list = list()
        self.t_list = list()
        self.W_list = list()
        self.Le_list = list()
        self.Re_list = list()
        self.init_environments()

    def init_environments(self):
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

    def init_mps_random(self):
        """Initialize a specific normalized mps with random entries and set all
        Schmidt values equal to one.
        Indices: [Mi]_ai-1 si ai, [Ai]_ai-1 si ai, [Bi+1]_ai si+1 ai+1, [li]_ ai
        """
        d = self.ldim
        L = self.N
        self.M_list = []
        self.s_list = []
        for _ in range(L):
            M = np.random.rand(d) + 1j * np.random.rand(d)
            M = M / (np.linalg.norm(M))
            M = M.reshape(1, d, 1)
            self.M_list.append(M)
            self.s_list.append(np.ones(1))
        self.A_list = self.M_list[:]
        self.B_list = self.M_list[:]

    def reshape_network(self):
        raise Exception("Not implemented.")
