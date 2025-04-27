#!/usr/bin/python3


class Mpo(object):
    """The class for matrix product operators (MPO)."""

    def __init__(self, hamiltonian_dict=None, operator_dict=None):
        self.hamiltonian_dict = hamiltonian_dict
        self.operator_dict = operator_dict
        self.alphabet = set()
        self.states = {"I", "F"}
        self.state_transistion_function = 0
        self.W_list = []

    def init_mpo(self):
        return 0

    # # predefined mpos ##########################################################
    # def init_pauli(self):
    #     """ Init local site operators: spin 1/2 -> pauli matrices. """
    #     I = np.eye(2)
    #     X = np.array([[0.,1.],[1.,0.]])
    #     Y = np.array([[0.,-1j],[1j,0.]])
    #     Z = np.array([[1.,0.],[0.,-1.]])
    #     return(I, X, Y, Z)

    # def init_mpo(self, params):
    #     if self.model_flag == "tfi":
    #         self.init_mpo_tfi(J=params["J"],h=params["h"])
    #     if self.model_flag == "hb":
    #         self.init_mpo_hb(J=params["J"],h=params["h"])

    # # heisenberg model
    # def init_mpo_hb(self, J=[], h=[]):
    #     # alias for common variables
    #     D = self.mdim
    #     d = self.ldim
    #     I, X, Y, Z = self.init_pauli()
    #     # init empty mpo list
    #     self.mpo_list = []
    #     # fill the mpo list
    #     for i in range(self.sites):
    #         W = np.zeros((D,D,d,d),dtype=complex)
    #         W[0,0:] = [I, J[i][0]*X, J[i][1]*Y, J[i][2]*Z, h[i][0]*X+h[i][1]*Y+h[i][2]*Z]
    #         W[1:,4] = [X, Y, Z, I]
    #         self.mpo_list.append(W)

    # # transverse field ising model
    # def init_mpo_tfi(self, J=[], h=[]):
    #     """ Initializing the matrix product operator. """
    #     # alias for common variables
    #     D = self.mdim
    #     d = self.ldim
    #     I, X, Y, Z = self.init_pauli()
    #     print(J,h)
    #     # init matrix product operator
    #     self.mpo_list = []
    #     for i in range(self.sites):
    #         W = np.zeros((D,D,d,d),dtype=complex)
    #         W[0,0:] = [I, -J[i]*X, h[i]*Z]
    #         W[1:,2] = [X, I]
    #         self.mpo_list.append(W)
    ########################################################################


#     def init_mpo_khm(self):
#         # init an empty container list for the mpo
#         self.mpo_list = [np.zeros((D, D, d, d), dtype=complex)]*int(Lx*Ly)
#
#         # unitary transition I->I, F->F
#         for i in range(int(Lx*Ly)):
#             self.mpo_list[i][0][0] = I; self.mpo_list[i][D-1][D-1] = I
#
#         # iterating through rules and fill paths to the matrix product operator list
#         for rule in transition_rules:
#             n = rule["n"] # current channel
#             x1, y1 = rule["I"][0]["i1"] # coordinates (xi, yi)
#             i = x1*Ly+y1 # transform into single index
#             if i>=0:
#                 self.mpo_list[i][0][n+1] = rule["p"] * rule["I"][0]["o1"] # fill MPO_list[site i] for transition I->node[channel n]
#             for k in range(len(rule["F"])): # iterate through final states
#                 x2, y2 = rule["F"][k]["i2"] # coordinates (xj, yj)
#                 j = x2*Ly+y2 # transform into single index
#                 if j<int(Lx*Ly):
#                     cd = 1 # covered distance
#                     for l in np.arange(i+1, j-1):
#                         self.mpo_list[l][(cd-1)*self.channels+n+1][cd*self.channels+n+1] = I
#                         cd += 1
#                     self.mpo_list[j][cd*self.channels+n][D-1] = rule["F"][k]["o2"]
