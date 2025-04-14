import numpy as np
import scipy as sp
import multiprocessing as mp

from nconpp.mps import MPS
from nconpp.mpo import MPO


class TEBD(MPS, MPO):
    """========================================================================
    The Time Evolving Block Decimation class. The default algorithm
    is the infinite form with a two-site update scheme. You can choose
    between the following algorithms by specifing the keyword "kind".
    ========================================================================
    Literature:
        G. Vidal: Efficient classical simulation of slightly entangled
            quantum computations, Phys. Rev. Lett. 91, 147902 (2003)
        G. Vidal: Efficient simulation of one-dimensional quantum many-body
            systems, Phys. Rev. Lett 93, 040502 (2004)
        J.J. Garcia-Ripoll: Time-Evolution with Matrix Product States
        M.L. Wall, L.D. Carr: Out of equlibrium dynamics with Matrix Product
            States
        S. Paeckel et. al.: Time-evolution methods for matrix-product states
            https://arxiv.org/abs/1901.05824
    ========================================================================
    Default Values:
        lattice: Class for the lattice
    ========================================================================
    Instance Variables:
    Key             |  Default          | Description
    ----------------+-------------------+-----------------------------------
    lattice         | optional, s.a.    | System Class
    algorithm       | itebd2s           | name of the algorithm
    chi_max         | 20                | maximal bond dimension
    disc_weight     | 1e-9              | discarded weight
    N_max           | 50                | maximal number of simulation steps
    checkpoint      | False             | do checkpointing
    checkpoint_step | 0                 | steps for checkpointing
    parallel        | False             | set parallelization
    nbp             | 1                 | number of processes
    nbt             | "not specified"   | number of threads
    step            | 0                 | the current simulation step
    result          | 0                 | the current result
    ========================================================================
    """

    def __init__(self, lattice=None):
        super()  # initilize inherit classes
        # default classes
        self.lattice = lattice  # the lattice class
        # simulation parameters
        self.algorithm = "itebd2s"  # name of the algorithm
        self.chi_min = 10  # minimal bond dimension
        self.chi_step = 10  # stepsize for increasing bond dimension
        self.chi_max = 50  # maximal bond dimension
        self.disc_weight = 1e-9  # discarded weight
        self.N_max = 50  # maximal number of simulation steps
        self.checkpoint = False  # do checkpointing
        self.checkpoint_step = 0  # steps for checkpointing
        self.parallel = False  # set parallelization during simulation
        self.nbp = 1  # number of processes
        self.nbt = "not specified"  # number of threads
        # tebd specific parameters
        self.eigensolver = "tebd"  # eigensolver, here block decimation
        self.time = "real"  # real or imag time
        self.dt = 0.01  # time step
        self.order = 1  # order of trotter decomposition
        self.U_list = []  # list for unitary time evolution blocks
        # temporary parameters
        self.step = 0  # the current simulation step
        self.result = 0  # the current result of the simulation

    # "public" functions #######################################################
    def init_unitaries_from_mpo(self):
        """Initialize the two-site blocks from the mpo for the unitary time
        evolution using the Suzuki-Trotter Decomposition.
        """
        try:
            # adapt length of list for unitaries to lattice size
            self.U_list = [0] * self.lattice.sites
            # init left and right boundary vectors
            vL = np.zeros((1, self.mdim, 1), dtype=complex)
            vL[0, 0, 0] = 1.0
            vR = np.zeros((1, self.mdim, 1), dtype=complex)
            vR[0, self.mdim - 1, 0] = 1.0
            # iterate through iterable_list
            for iterables in self.lattice.iterable_list:
                for indices in iterables:
                    # indices
                    iWL = indices[1]
                    W0 = self.W_list[iWL]
                    iWR = indices[2]
                    W1 = self.W_list[iWR]
                    # set up unitaries
                    U = np.tensordot(vL, W0, axes=(1, 0))
                    U = np.tensordot(U, W1, axes=(2, 0))
                    U = np.tensordot(U, vR, axes=(4, 1))
                    U = np.squeeze(U)  # si sip si1 si1p
                    U = np.transpose(U, (0, 2, 1, 3))  # si si1 sip si1p
                    Ushape = U.shape
                    U = np.reshape(U, (Ushape[0] * Ushape[1], Ushape[2] * Ushape[3]))
                    if self.time == "real":
                        U = sp.linalg.expm(-1j * self.dt * U)
                    if self.time == "imag":
                        U = sp.linalg.expm(-self.dt * U)
                    U = np.reshape(U, (Ushape[0], Ushape[1], Ushape[2], Ushape[3]))
                    self.U_list[iWL] = U
        except:
            if self.W_list == None:
                print("Could not find Mpo List.")

    def simulation_dict(self):
        """Return simulation parameters as dictionary."""
        return {
            "algorithm": self.algorithm,
            "chi_min": self.chi_min,
            "chi_step": self.chi_step,
            "chi_max": self.chi_max,
            "disc_weight": self.disc_weight,
            "N_max": self.N_max,
            "checkpoint": self.checkpoint,
            "checkpoint_step": self.checkpoint_step,
            "parallel": self.parallel,
            "nbp": self.nbp,
            "nbt": self.nbt,
        }

    def tebd_dict(self):
        """Return the current dmrg parameters as dictionary."""
        return {
            "timestep": self.dt,
            "time": self.time,
            "trotterorder": self.order,
            "U_list": self.U_list,
        }

    def collect_data(self):
        """Collect the current data in a big dictionary."""
        return {
            "simulation": {**self.simulation_dict(), **self.tebd_dict()},
            "eigensolver": self.eigensolver,
            "mps": self.mps_dict(),
            "mpo": self.mpo_dict(),
            "lattice": self.lattice,
        }

    def _contract_theta(self, l0, M1, l1, M2, l2):
        """Contracting bond parameter to theta."""
        theta = np.tensordot(np.diag(l0), M1, axes=(1, 0))
        theta = np.tensordot(theta, np.diag(l1), axes=(2, 0))
        theta = np.tensordot(theta, M2, axes=(2, 0))
        return np.tensordot(theta, np.diag(l2), axes=(3, 0))

    def _checkrun(self):
        """Checks if everything is ready for the simulation."""
        check = True
        if None in self.mps_dict().values():
            check = False
            if self.M_list != None or self.A_list != None or self.B_list != None:
                check = True
        if None in vars(self).values():
            check = False
        if check == False:
            print("Simulation parameters are not ready.")
        return check

    def print_checkrun(self):
        """Check for "None" in classes."""
        if None in self.mps_dict().values():
            for i, j in self.mps_dict().items():
                if j == None:
                    print("{} hasn't be set.".format(i))
        if None in vars(self).values():
            print("You haven't set everything yet:")
            for i, j in vars(self).items():
                if j == None:
                    print("{} hasn't be set.".format(i))

    def run(self):
        """Starts the simulation if checkrun() returns True."""
        if self._checkrun():
            self.step += 1
            if self.parallel == False:
                # perform the iTEBD calculation sequential
                self.step = 0
                while self.step <= self.N_max:
                    # iterating through system
                    for iterables in self.lattice.iterable_list:
                        for indices in iterables:
                            self.result = self._itebd2s(indices)
                            self._update()
                    if (
                        self.checkpoint == True
                        and self.checkpoint_step % self.step == 0
                    ):
                        yield self.collect_data()
                    self.step += 1
            elif self.parallel:
                # perform the iTEBD calculation in parallel
                if self.nbp > mp.cpu_count():
                    print(
                        "You want to use more processes then cores available. This doesnt affect Pool(), but your number of processes is bounded by the max number of available cpus, setting nbp = mp.cpu_count()."
                    )
                    self.nbp = mp.cpu_count()
                # perform the iTEBD calculation
                with mp.Pool(self.nbp) as pool:
                    while self.step < self.N_max:
                        print(self.step)
                        for iterables in self.lattice.iterable_list:
                            # pickle cannot be used with class methods,
                            # workaround:
                            # putting a function outside of the class
                            # which unwraps self of the class
                            # and give it as argument to the method of the class
                            # called as an outer function
                            # self.result = pool.map(unwrap_self_function, zip([self]*len(iterables), iterables))
                            self.result = pool.imap(self._itebd2s, iterables)
                            # update mps
                            for result in self.result:
                                self._update(result)

    def _itebd2s(self, iterable):
        """The iDMRG "distributor" function, sequential."""
        # get indices
        iL = iterable[0]
        iWL = iterable[1]
        iWR = iterable[2]
        iR = iterable[3]
        # get mps matrices
        l0 = self.l_list[iL]
        B1 = self.B_list[iWL]
        l1 = self.l_list[iWL]
        A2 = self.A_list[iWR]
        l2 = self.l_list[iWR]
        # get U matrix
        Ut = self.U_list[iWL]
        # call the worker function
        U, S, V = self._itebd2s_worker(l0, B1, l1, A2, l2, Ut)
        return iL, iWL, iWR, iR, U, S, V

    def _itebd2s_worker(self, l0, V1, l1, U2, l2, Ut):
        """The worker function for the two-site idmrg."""
        # get bond dimensions and ...
        chi0 = l0.shape[0]
        chi2 = l2.shape[0]
        # ... local dimensions of mps
        d1 = V1.shape[1]
        d2 = U2.shape[1]
        # McCulloch's wave function prediction
        theta = self._wave_function_prediction(l0, V1, l1, U2, l2)
        # apply U
        theta = np.tensordot(Ut, theta, axes=([2, 3], [1, 2]))
        # matricization of theta
        theta = np.reshape(theta, (chi0 * d1, d2 * chi2))
        # performing singular value decomposition
        U, S, V = np.linalg.svd(theta, full_matrices=0)
        # truncate bond dimension if needed
        chi = np.min([np.sum(S > self.disc_weight), self.chi_max])
        # reshape U,V to new mps, normalize truncated singular values
        U = U[:, :chi]
        U = np.reshape(U, (chi0, d1, chi))
        S = S[:chi] / np.linalg.norm(S[:chi])
        V = V[:chi, :]
        V = np.reshape(V, (chi, d2, chi2))
        return U, S, V

    def _update(self, result=None):
        """Updates the MPS, the environments and bond energies. The environments
        are getting always updated by the left and right canonical matrices from
        the calculation. Whereas the matrix product state is updated by the specific
        configuration in the mps class.
        """
        if result != None:
            self.result = result
        # ... get indices, ...
        iWL = self.result[1]
        iWR = self.result[2]
        # ... the matrices, schmidt values for the matrix product state
        self.A_list[iWL] = self.result[4]
        self.l_list[iWL] = self.result[5]
        self.B_list[iWR] = self.result[6]
