import numpy as np
import multiprocessing as mp

from tensornetwork import TENSORNETWORK


class DMRG(TENSORNETWORK):
    """The Density Matrix Renormalization Group in MPS formalism

    Args:
        TENSORNETWORK: base class for tensor network simulations, it includes:
            * LATTICE: base class for a lattice graph implementation
            * Attributes for matrix product states, operator and environments.

    Attributes:
        step (int):
            current simulation step
            default 0
        chi_min (int)
            minimal bond dimension
            default 10
        chi_max (int):
            maximal bond dimension
            default 50
        chi_delta (int)
            increment for bond dimension
            default 10
        chi_step (int)
            current bond dimension
            default chi_min
        disc_weight (float)
            discarded weight
            default 1e-9
        step_pre (int)
            pre run steps without diagonalization
            default 5
        step_max (int)
            maximal number of simulation steps
            default 50
        checkpoint (bool)
            do checkpointing, probably high memory throughput
            default False
        checkpoint_step
            steps for checkpointing
            default 0
        parallel
            set parallelization
            default False
        nbp
            number of processors to use
            default 1
        result
            current result object
        E_list list(float):
            list for current energies on the bonds
        Estep_list list(float):
            list for energies on the bonds during simulation
    """

    def __init__(self):
        super().__init__()
        self.step = 0
        self.chi_min = 10
        self.chi_max = 50
        self.chi_step = 10
        self.chi_delta = 10
        self.disc_weight = 1e-9
        self.step_pre = 0
        self.step_max = 50
        self.checkpoint = False
        self.checkpoint_step = 0
        self.parallel = False
        self.nbp = 1
        self.E_list = []
        self.Estep_list = []
        self.result = None

    def simulation_dict(self):
        """Return simulation parameters as dictionary."""
        return {
            "step": self.step,
            "chi_min": self.chi_min,
            "chi_max": self.chi_max,
            "chi_delta": self.chi_delta,
            "chi_step": self.chi_step,
            "disc_weight": self.disc_weight,
            "step_pre": self.step_pre,
            "step_max": self.step_max,
            "checkpoint": self.checkpoint,
            "checkpoint_step": self.checkpoint_step,
            "parallel": self.parallel,
            "nbp": self.nbp,
        }

    def dmrg_dict(self):
        """Return the current dmrg parameters as dictionary."""
        return {"En_list": self.En_list}

    def collect_data(self):
        """Collect the current data in a big dictionary."""
        return {
            "simulation": {**self.simulation_dict(), **self.dmrg_dict()},
            "mps": self.mps_dict(),
            "mpo": self.mpo_dict(),
            "lattice": self.lattice,
        }

    def print_check_sim(self):
        """Check for "None" in classes."""
        if None in self.mps_dict().values():
            for i, j in self.mps_dict().items():
                if j is None:
                    print("{} hasn't be set.".format(i))
        if None in self.mpo_dict().values():
            print("You haven't set everything yet:")
            for i, j in self.mpo_dict().items():
                if j is None:
                    print("{} hasn't be set.".format(i))
        if None in vars(self).values():
            print("You haven't set everything yet:")
            for i, j in vars(self).items():
                if j is None:
                    print("{} hasn't be set.".format(i))

    def init_env(self):
        """Initialize left and right environments as unities.
        [Li]_ai bi aip, [Ri]_ai-1 bi-1 ai-1p
        """
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

    def run(self, printout=False, printstep=5):
        """Starts the simulation if checkrun() returns True."""
        if self._check_sim():
            # warmup runs
            self._warmup()
            # adapting energy list to lattice size
            self.E_list = [0] * self.lattice.tot_sites
            # Hida infinite-size DMRG ...
            if not self.parallel:
                # ... sequential
                self.step = 0
                while self.step < self.step_max:
                    for iterables in self.lattice.iterable_list:
                        for indices in iterables:
                            self.result = self._idmrg2s(indices)
                            self._update_dmrg()
                    self.En_list.append(self.E_list[:])
                    if printout and self.step % printstep == 0:
                        self._print_statistics()
                    if self.checkpoint and self.checkpoint_step % self.step == 0:
                        yield self.collect_data()
                    self.step += 1
            elif self.parallel:
                # ...parallel
                if self.nbp > mp.cpu_count():
                    print(
                        "You want to use more processors then cores available. \
                        This doesnt affect Pool(), but your number of processes \
                        is bounded by the max number of available cpus,  \
                        setting nbp = mp.cpu_count()."
                    )
                    self.nbp = mp.cpu_count()
                with mp.Pool(self.nbp) as pool:
                    while self.step < self.step_max:
                        print(self.step)
                        for iterables in self.lattice.iterable_list:
                            self.result = pool.imap(self._idmrg2s, iterables)
                            # update mps, environments, bond energies
                            self._update_dmrg()
                        self.step += 1

    def bond_energy(self):
        """Returning the current bond energy by stacking Le l Re."""
        L = self.lattice.tot_sites
        bond_energy_list = []
        for i in range(L):
            en = np.tensordot(self.Le_list[i], np.diag(self.l_list[i]), axes=(0, 0))
            en = np.tensordot(en, np.diag(self.l_list[i]), axes=(1, 0))
            en = np.tensordot(
                en,
                self.Re_list[(i + 1) % self.lattice.tot_sites],
                axes=([1, 0, 2], [0, 1, 2]),
            )
            bond_energy_list.append(en)
        return bond_energy_list

    def _check_sim(self):
        """Checks if everything is ready for the simulation."""
        check = True
        if None in self.mps_dict().values():
            check = False
            if (
                self.M_list is not None
                or self.A_list is not None
                or self.B_list is not None
            ):
                check = True
        if None in self.mpo_dict().values():
            check = False
        if None in vars(self).values():
            check = False
        if not check:
            print("Simulation is not ready. Some parameters hasn't be set.")
        return check

    def _print_statistics(self):
        L = self.lattice.tot_sites
        if self.step == 0:
            print("# step \t energy \t bond dimension")
        elif self.step > L:
            print(
                self.step,
                "\t",
                [
                    -abs(self.En_list[self.step][i] - self.En_list[self.step - L][i])
                    / (4 * L)
                    for i in range(L)
                ],
                "\t",
                [len(l) for l in self.l_list],
            )

    def _warmup(self):
        """Warmup runs for iDMRG. Setup A_list and B_list of the MPS. This
        function is equally to idmrg2s without performing an optimization
        of the matrix product state.
        """
        L = self.lattice.tot_sites  # local shortcut
        if self.M_list is not None:
            self.A_list = self.M_list[:]
            self.B_list = self.M_list[:]
        step = 0
        while step < self.step_pre:
            for i in range(L):
                # get indices
                iL = i
                iR = (i + 1) % L
                # get mps matrices
                l0 = self.l_list[iL - 1]
                V1 = self.B_list[iL]
                l1 = self.l_list[iL]
                U2 = self.A_list[iR]
                l2 = self.l_list[iR]
                # get bond dimensions and ...
                chi0 = l0.shape[0]
                chi2 = l2.shape[0]
                # ... local dimensions of mps
                d1 = V1.shape[1]
                d2 = U2.shape[1]
                # wave function prediction
                theta0 = self._wave_functiostep_prediction(l0, V1, l1, U2, l2)
                # matricization of theta0
                theta0 = np.reshape(theta0, (chi0 * d1, d2 * chi2))
                # performing singular value decomposition
                U, S, V = np.linalg.svd(theta0, full_matrices=0)
                # truncate bond dimension if needed
                chi = np.min([np.sum(S > self.disc_weight), self.chi_max])
                # reshape U,V to new mps, normalize truncated singular values
                U = U[:, :chi]
                U = np.reshape(U, (chi0, d1, chi))
                S = S[:chi] / np.linalg.norm(S[:chi])
                V = V[:chi, :]
                V = np.reshape(V, (chi, d2, chi2))
                # update mps matrices ...
                self.A_list[iL] = U
                self.l_list[iL] = S
                self.B_list[iR] = V
                # ... update left ...
                self.Le_list[iL] = np.tensordot(self.Le_list[iL - 1], U, axes=(0, 0))
                self.Le_list[iL] = np.tensordot(
                    self.Le_list[iL], self.W_list[iL], axes=([0, 2], [0, 2])
                )
                self.Le_list[iL] = np.tensordot(
                    self.Le_list[iL], np.conj(U), axes=([0, 3], [0, 1])
                )
                # ...  and right environments and ...
                self.Re_list[iR] = np.tensordot(
                    self.Re_list[(iR + 1) % L], V, axes=(0, 2)
                )
                self.Re_list[iR] = np.tensordot(
                    self.Re_list[iR], self.W_list[iR], axes=([0, 3], [1, 2])
                )
                self.Re_list[iR] = np.tensordot(
                    self.Re_list[iR], np.conj(V), axes=([0, 3], [2, 1])
                )
            step += 1
        return (self.A_list, self.l_list, self.B_list)

    def _update_dmrg(self):
        """Updates the MPS, the environments and bond energies. The environments
        are getting always updated by the left and right canonical matrices from
        the calculation. Whereas the matrix product state is updated by the specific
        configuration in the mps class.
        """
        if not self.parallel:
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
        iL = result[0]
        iWL = result[1]
        iWR = result[2]
        iR = result[3]
        # ... update left ...
        self.Le_list[iWL] = np.tensordot(self.Le_list[iL], self.result[4], axes=(0, 0))
        self.Le_list[iWL] = np.tensordot(
            self.Le_list[iWL], self.W_list[iWL], axes=([0, 2], [0, 2])
        )
        self.Le_list[iWL] = np.tensordot(
            self.Le_list[iWL], np.conj(self.result[4]), axes=([0, 3], [0, 1])
        )
        # ...  and right environments ...
        self.Re_list[iWR] = np.tensordot(self.Re_list[iR], self.result[6], axes=(0, 2))
        self.Re_list[iWR] = np.tensordot(
            self.Re_list[iWR], self.W_list[iWR], axes=([0, 3], [1, 2])
        )
        self.Re_list[iWR] = np.tensordot(
            self.Re_list[iWR], np.conj(self.result[6]), axes=([0, 3], [2, 1])
        )

    def _idmrg2s(self, iterable):
        """The iDMRG sequential "distributor" function."""
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
        """The worker function for the two-site idmrg."""
        # get bond dimensions and ...
        chi0 = l0.shape[0]
        chi2 = l2.shape[0]
        # ... local dimensions of mps
        d1 = V1.shape[1]
        d2 = U2.shape[1]
        # McCulloch's wave function prediction
        theta0 = self._wave_function_prediction(l0, V1, l1, U2, l2)
        # diagonalizing the projected bond hamiltonian
        E0, theta = self.eigensolver._diag(
            L0,
            W1,
            W2,
            R3,
            theta0,
            k=self.eigensolver.num_vec,
            eigensolver=self.eigensolver.which,
        )
        # matricization of theta
        theta = np.reshape(theta[:, 0], (chi0 * d1, d2 * chi2))
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
        return U, S, V, E0

    def _wave_function_prediction(self, l0, V1, l1, U2, l2):
        """Wave Function Prediction by I.P. McCulloch, e.g. l0xM1xl1xM2xl2
        ========================================================================
        Args:
            l0_(a0, a0), l1_(a1, a1), l2_(a2, a2): Schmidt Values
            V1_(a0, s1, a1), U2_(a1, s2, a2): Matrix Product States
        Returns:
            theta_(a0, s1, s2, a2): contracted two-site wave function theta
        """
        theta = np.tensordot(np.diag(l0), V1, axes=(1, 0))
        theta = np.tensordot(theta, np.diag(l1 ** (-1)), axes=(2, 0))
        theta = np.tensordot(theta, U2, axes=(2, 0))
        theta = np.tensordot(theta, np.diag(l2), axes=(3, 0))
        return theta


"""     # TODO
    def _contract_left(self,L,M):

    def _contract_right(self,R,M): """
