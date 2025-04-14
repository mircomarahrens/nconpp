import numpy as np

from .lattice import LATTICE

class MPS(LATTICE):
    """ ========================================================================
        The class for matrix product states (MPS).
        ========================================================================
        Inherit classes: LATTICE
        ========================================================================
        Key             |  Default        | Description
        ----------------+-----------------+-------------------------------------
        ldim            | 2               | physical dimension
        A_list          | None            | left canonical mps
        B_list          | None            | right canonical mps
        l_list          | None            | Schmidt values
        M_list          | None            | initial mps
        t_list          | None            | eigenvalues of the transfer matrix
        ========================================================================
    """

    def __init__(self, local_dim=None):
        super().__init__()    # initilize inherit classes
        self.ldim = local_dim # physical dimension of local hilbert space
        self.A_list = None    # list for left canonical MPS
        self.B_list = None    # list for right canonical MPS
        self.l_list = None    # list for Schmidt values
        self.M_list = None    # list for canonical MPS (Vidal representation)
        self.t_list = None    # list for eigenvalues of transfer matrix
        

    def init_mps(self):
        """ Shortcut for the initialization of the MPS. Maybe it is useful later
        to add other kinds of initialization than random. """
        return self.init_random()

    def init_random(self):
        """ Initialize a specific normalized mps with random entries and set all
        Schmidt values equal to one.
        Indices: [Mi]_ai-1 si ai, [Ai]_ai-1 si ai, [Bi+1]_ai si+1 ai+1, [li]_ ai
        """
        d = self.ldim
        L = self.N
        self.M_list = []; self.l_list = []
        for _ in range(L):
            M = np.random.rand(d)+1j*np.random.rand(d)
            M = M/(np.linalg.norm(M))
            M = M.reshape(1,d,1)
            self.M_list.append(M)
            self.l_list.append(np.ones(1))
        self.A_list = self.M_list[:]
        self.B_list = self.M_list[:]

    def update(self, result):
        # get indices, ...
        iWL = result[1]; iWR = result[2]
        # ...and update the matrices and Schmidt values for the matrix product state
        self.A_list[iWL] = result[4]; self.l_list[iWL] = result[5]; self.B_list[iWR] = result[6]

    def _wave_function_prediction(self, l0, V1, l1, U2, l2):
        """ Wave Function Prediction by I.P. McCulloch, e.g. l0xM1xl1xM2xl2
        ========================================================================
        Args:
            l0_(a0, a0), l1_(a1, a1), l2_(a2, a2): Schmidt Values
            V1_(a0, s1, a1), U2_(a1, s2, a2): Matrix Product States
        Returns:
            theta_(a0, s1, s2, a2): contracted two-site wave function theta
        """
        theta = np.tensordot(np.diag(l0), V1, axes=(1,0))
        theta = np.tensordot(theta, np.diag(l1**(-1)), axes=(2,0))
        theta = np.tensordot(theta, U2, axes=(2,0))
        theta = np.tensordot(theta, np.diag(l2), axes=(3,0))
        return theta