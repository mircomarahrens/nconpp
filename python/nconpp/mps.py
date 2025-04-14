#!/usr/bin/python3

import numpy as np


class Mps(object):
    """description of class"""

    def __init__(self, sites=(4,), local_dimension=2, coordination_number=2):
        super.__init__()
        self.A_list = list()
        self.B_list = list()
        self.s_list = list()
        self.M_list = list()
        self.ldim = local_dimension
        self.sites = sites

    def init_mps_random(self):
        """Initialize a normalized mps with random entries and set all
        Schmidt values equal to one.
        """
        d = self.ldim
        L = self.sites
        self.M_list = []
        self.s_list = []
        for _ in range(L):
            M = np.random.rand(d) + 1j * np.random.rand(d)
            M = M / (np.linalg.norm(M))
            M = M.reshape(d, 1, 1)
            self.M_list.append(M)
            self.s_list.append(np.ones(1))
        self.A_list = self.M_list[:]
        self.B_list = self.M_list[:]
