import numpy as np
import multiprocessing as mp

from tnb.modules.mps import MPS
from tnb.modules.mpo import MPO

# TODO add possibility to load precalculated data, finite dmrg: contract_left, contract_right

class DMRG(MPS, MPO):
    """ ========================================================================
        The Density Matrix Renormalization Group in the Matrix Product 
        Formalism. The default algorithm uses the infinite form with a two-site 
        update gate.
        ========================================================================
        Literature:
            U. Schollwoeck: The density matrix renormalization group in the 
                age of matrix product states, https://arxiv.org/abs/1008.3477
            H. Ueda: Infinite-size density matrix renormalization group with
                parallel Hida's algorithm, https://arxiv.org/abs/1606.06790
        ========================================================================
        Inherit classes: MPS, MPO
        ========================================================================
        Key             |  Default        | Description
        ----------------+-----------------+-------------------------------------
        eigensolver     | None            | Eigensolver Class
        step            | 0               | the current simulation step
        chi_min         | 10              | minimal bond dimension
        chi_max         | 50              | maximal bond dimension
        chi_delta       | 10              | increment for bond dimension
        chi_step        | 10              | current bond dimension
        disc_weight     | 1e-9            | discarded weight
        N_pre           | 5               | pre run without diagonalization
        N_max           | 50              | maximal number of simulation steps
        checkpoint      | False           | do checkpointing
        checkpoint_step | 0               | steps for checkpointing
        parallel        | False           | set parallelization
        nbp             | 1               | number of processors to use
        Le_list         | None            | list for left environments
        Re_list         | None            | list for right environments
        En_list         | []              | full bond energy list per step
        E_list          | []              | current bond energy list
        result          | None            | the current result
        form            | "infinite       | ininite or finite system
        ========================================================================
    """

    def __init__(self, eigensolver=None, model=None):
        if model==None:
            super().__init__()                    # initilize inherit classes
        else:
            model.__init__()
        # init parameters
        self.step            = 0                  # the current simulation step
        self.chi_min         = 10                 # minimal bond dimension
        self.chi_max         = 50                 # maximal bond dimension
        self.chi_step        = 10                 # current bond dimension
        self.chi_delta       = 10                 # stepsize for increasing bond dimension
        self.disc_weight     = 1e-9               # discarded weight
        self.N_pre           = 0                  # pre run without diagonalization
        self.N_max           = 50                 # maximal number of simulation steps
        self.checkpoint      = False              # do checkpointing -> large output!
        self.checkpoint_step = 0                  # steps for checkpointing
        self.parallel        = False              # using parallelization
        self.nbp             = 1                  # number of processes
        # dmrg specific parameters
        self.Le_list         = None               # list for left environments
        self.Re_list         = None               # list for right environments
        self.En_list         = []                 # energy list during calculation
        self.E_list          = []                 # current energy list
        self.result          = None               # the current result of the simulation
        self.form            = "infinite"         # infinite or finite systems


    # "public" functions ########################################################
    def simulation_dict(self):
        """ Return simulation parameters as dictionary. """
        return({"step":            self.step,
                "chi_min":         self.chi_min,
                "chi_max":         self.chi_max,
                "chi_delta":       self.chi_delta,
                "chi_step":        self.chi_step,
                "disc_weight":     self.disc_weight,
                "N_pre":           self.N_pre,
                "N_max":           self.N_max,
                "checkpoint":      self.checkpoint,
                "checkpoint_step": self.checkpoint_step,
                "parallel":        self.parallel,
                "nbp":             self.nbp})

    def dmrg_dict(self):
        """ Return the current dmrg parameters as dictionary. """
        return({"Le_list": self.Le_list,
                "Re_list": self.Re_list,
                "En_list": self.En_list})

    def collect_data(self):
        """ Collect the current data in a big dictionary. """
        return({"simulation":  {**self.simulation_dict(), 
                                **self.dmrg_dict()},
                "mps":         self.mps_dict(),
                "mpo":         self.mpo_dict(),
                "lattice":     self.lattice})

    def print_check_sim(self):
        """ Check for "None" in classes. """
        if None in self.mps_dict().values():
            for i,j in self.mps_dict().items():
                if j==None:
                    print("{} hasn't be set.".format(i))
        if None in self.mpo_dict().values():
            print("You haven't set everything yet:")
            for i,j in self.mpo_dict().items():
                if j==None:
                    print("{} hasn't be set.".format(i))
        if None in vars(self).values():
            print("You haven't set everything yet:")
            for i,j in vars(self).items():
                if j==None:
                    print("{} hasn't be set.".format(i))

    def init_env(self):
        """ Initialize left and right environments as unities. 
            [Li]_ai bi aip, [Ri]_ai-1 bi-1 ai-1p
        """
        W_list = self.W_list
        M_list = self.M_list
        L = len(W_list)
        self.Le_list = []; self.Re_list = []
        Le = np.zeros((1, self.wdim, 1), dtype=complex); Le[0,0,0] = 1.0
        Re = np.zeros((1, self.wdim, 1), dtype=complex); Re[0,self.wdim-1,0] = 1.0
        for i in range(L):
            Le = np.tensordot(Le, M_list[i], axes=(0,0))
            Le = np.tensordot(Le, W_list[i], axes=([0,2],[0,3]))
            Le = np.tensordot(Le, np.conj(M_list[i]), axes=([0,3],[0,1]))
            Re = np.tensordot(Re, M_list[-i-1], axes=(0,2))
            Re = np.tensordot(Re, W_list[-i-1], axes=([0,3],[1,3]))
            Re = np.tensordot(Re, np.conj(M_list[-i-1]), axes=([1,3],[2,1]))
            self.Le_list.append(Le); self.Re_list = [Re] + self.Re_list
        return(self.Le_list, self.Re_list)

    def run(self, printout=False, printstep=5):
        """ Starts the simulation if checkrun() returns True. """
        if self._check_sim():
            # warmup runs
            self._warmup()
            # adapting energy list to lattice size
            self.E_list = [0]*self.lattice.tot_sites
            # Hida infinite-size DMRG ...
            if self.parallel==False:
                # ... sequential
                self.step = 0
                while self.step<self.N_max:
                    for iterables in self.lattice.iterable_list:
                        for indices in iterables:
                            self.result = self._idmrg2s(indices)
                            self._update_dmrg()
                    self.En_list.append(self.E_list[:])
                    if printout==True and self.step%printstep == 0:
                        self._print_statistics()
                    if self.checkpoint == True and self.checkpoint_step%self.step==0:
                        yield self.collect_data()
                    self.step += 1
            elif self.parallel:
                # ...parallel
                if self.nbp > mp.cpu_count():
                    print("You want to use more processors then cores available. \
                        This doesnt affect Pool(), but your number of processes \
                        is bounded by the max number of available cpus,  \
                        setting nbp = mp.cpu_count().")
                    self.nbp = mp.cpu_count()
                with mp.Pool(self.nbp) as pool:
                    while self.step<self.N_max:
                        print(self.step)
                        for iterables in self.lattice.iterable_list:
                            self.result = pool.imap(self._idmrg2s, iterables)
                            # update mps, environments, bond energies
                            self._update_dmrg()
                        self.step+=1

    def bond_energy(self):
        """ Returning the current bond energy by stacking Le l Re. """
        L = self.lattice.tot_sites
        bond_energy_list = []
        for i in range(L):
            en = np.tensordot(self.Le_list[i], np.diag(self.l_list[i]), axes=(0,0))
            en = np.tensordot(en, np.diag(self.l_list[i]), axes=(1,0))
            en = np.tensordot(en, self.Re_list[(i+1)%self.lattice.tot_sites], axes=([1,0,2],[0,1,2]))
            bond_energy_list.append(en)
        return bond_energy_list
    #############################################################################

    # "private" functions #######################################################
    def _check_sim(self):
        """ Checks if everything is ready for the simulation. """
        check = True
        if None in self.mps_dict().values():
            check = False
            if self.M_list!=None or self.A_list!=None or self.B_list!=None:
                check = True
        if None in self.mpo_dict().values():
            check = False
        if None in vars(self).values():
            check = False
        if check == False: print("Simulation is not ready. Some parameters hasn't be set.")
        return check

    def _print_statistics(self):
        L = self.lattice.tot_sites
        if self.step==0:
            print("# step \t energy \t bond dimension")
        elif self.step>L:
            print(self.step, "\t", [-abs(self.En_list[self.step][i]-self.En_list[self.step-L][i])/(4*L) for i in range(L)], "\t", [len(l) for l in self.l_list])

    def _warmup(self):
        """ Warmup runs for iDMRG. Setup A_list and B_list of the MPS. This
        function is equally to idmrg2s without performing an optimization
        of the matrix product state.
        """
        L = self.lattice.tot_sites # local shortcut
        if self.M_list!=None:
            self.A_list = self.M_list[:]
            self.B_list = self.M_list[:]
        step = 0
        while step<self.N_pre:
            for i in range(L):
                # get indices
                iL = i; iR = (i+1)%L
                # get mps matrices
                l0 = self.l_list[iL-1]
                V1 = self.B_list[iL]
                l1 = self.l_list[iL]
                U2 = self.A_list[iR]
                l2 = self.l_list[iR]
                # get bond dimensions and ...
                chi0 = l0.shape[0]; chi2 = l2.shape[0]
                # ... local dimensions of mps
                d1 = V1.shape[1]; d2 = U2.shape[1]
                # wave function prediction
                theta0 = self._wave_function_prediction(l0, V1, l1, U2, l2)
                # matricization of theta0
                theta0 = np.reshape(theta0, (chi0*d1,d2*chi2))
                # performing singular value decomposition
                U, S, V = np.linalg.svd(theta0, full_matrices = 0)
                # truncate bond dimension if needed
                chi = np.min([np.sum(S>self.disc_weight), self.chi_max])
                # reshape U,V to new mps, normalize truncated singular values
                U = U[:,:chi]; U = np.reshape(U,(chi0,d1,chi))
                S = S[:chi]/np.linalg.norm(S[:chi])
                V = V[:chi,:]; V = np.reshape(V,(chi,d2,chi2))
                # update mps matrices ...
                self.A_list[iL] = U; self.l_list[iL] = S; self.B_list[iR] = V
                # ... update left ...
                self.Le_list[iL] = np.tensordot(self.Le_list[iL-1], U, axes=(0,0))
                self.Le_list[iL] = np.tensordot(self.Le_list[iL], self.W_list[iL], axes=([0,2],[0,2]))
                self.Le_list[iL] = np.tensordot(self.Le_list[iL], np.conj(U), axes=([0,3],[0,1]))
                # ...  and right environments and ...
                self.Re_list[iR] = np.tensordot(self.Re_list[(iR+1)%L], V, axes=(0,2))
                self.Re_list[iR] = np.tensordot(self.Re_list[iR], self.W_list[iR], axes=([0,3],[1,2]))
                self.Re_list[iR] = np.tensordot(self.Re_list[iR], np.conj(V), axes=([0,3],[2,1]))
            step += 1
        return(self.A_list, self.l_list, self.B_list)

    def _update_dmrg(self):
        """ Updates the MPS, the environments and bond energies. The environments
            are getting always updated by the left and right canonical matrices from
            the calculation. Whereas the matrix product state is updated by the specific
            configuration in the mps class.
        """
        if self.parallel == False:
            # update mps and ...
            self._update_mps(self.result)
            # ... environments and ...
            self._update_env(self.result)
            # ... bond energies.
            self.E_list[self.result[1]] = self.result[7]
        elif self.parallel:
            for result in self.result:
                # update mps and ...
                self._update_mps(result)
                # ... environments and ...
                self._update_env(result)
                # ... bond energies.
                self.E_list[result[1]] = result[7]

    def _update_env(self, result):
        # get indices and ...
        iL  = result[0]; iWL = result[1]; iWR = result[2]; iR  = result[3]
        # ... update left ...
        self.Le_list[iWL] = np.tensordot(self.Le_list[iL], self.result[4], axes=(0,0))
        self.Le_list[iWL] = np.tensordot(self.Le_list[iWL], self.W_list[iWL], axes=([0,2],[0,2]))
        self.Le_list[iWL] = np.tensordot(self.Le_list[iWL], np.conj(self.result[4]), axes=([0,3],[0,1]))
        # ...  and right environments ...
        self.Re_list[iWR] = np.tensordot(self.Re_list[iR], self.result[6], axes=(0,2))
        self.Re_list[iWR] = np.tensordot(self.Re_list[iWR], self.W_list[iWR], axes=([0,3],[1,2]))
        self.Re_list[iWR] = np.tensordot(self.Re_list[iWR], np.conj(self.result[6]), axes=([0,3],[2,1]))

    def _idmrg2s(self, iterable):
        """ The iDMRG sequential "distributor" function. """
        # get indices
        iL = iterable[0]
        iWL = iterable[1]
        iWR = iterable[2]
        iR = iterable[3]
        # get mps matrices
        l0 = self.l_list[iL]
        V1 = self.B_list[iWL]
        l1 = self.l_list[iWL]
        U2 = self.A_list[iWR]
        l2 = self.l_list[iWR]
        # get mpo matrices
        W1 = self.W_list[iWL]
        W2 = self.W_list[iWR]
        # get environments
        L0 = self.Le_list[iL]
        R3 = self.Re_list[iR]
        # call the worker function
        U, S, V, Ei = self._idmrg2s_worker(l0, V1, l1, U2, l2, L0, W1, W2, R3)
        return iL, iWL, iWR, iR, U, S, V, Ei

    def _idmrg2s_worker(self, l0, V1, l1, U2, l2, L0, W1, W2, R3):
        """ The worker function for the two-site idmrg. """
        # get bond dimensions and ...
        chi0 = l0.shape[0]; chi2 = l2.shape[0]
        # ... local dimensions of mps
        d1 = V1.shape[1]; d2 = U2.shape[1]
        # McCulloch's wave function prediction
        theta0 = self._wave_function_prediction(l0, V1, l1, U2, l2)
        # diagonalizing the projected bond hamiltonian
        E0, theta = self.eigensolver._diag(L0, W1, W2, R3, theta0,
            k=self.eigensolver.num_vec, eigensolver=self.eigensolver.which)
        # matricization of theta
        theta = np.reshape(theta[:,0], (chi0*d1,d2*chi2))
        # performing singular value decomposition
        U, S, V = np.linalg.svd(theta, full_matrices = 0)
        # truncate bond dimension if needed
        chi = np.min([np.sum(S>self.disc_weight), self.chi_max])
        # reshape U,V to new mps, normalize truncated singular values
        U = U[:,:chi]; U = np.reshape(U,(chi0,d1,chi))
        S = S[:chi]/np.linalg.norm(S[:chi])
        V = V[:chi,:]; V = np.reshape(V,(chi,d2,chi2))
        return U, S, V, E0
        #########################################################################

"""     # TODO
    def _contract_left(self,L,M):

    def _contract_right(self,R,M): """

