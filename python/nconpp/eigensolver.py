#!/usr/bin/python3

import numpy as np
import python.nconpp.linalg as linalg


class Eigensolver:
    """The Eigensolver class. This is our main method for diagonalization.
    ============================================================================
    Attributes:
        which: which eigensolver to use?
        lanczos
            _min: minimal number of lanczos runs
            _max: maximal number of lanczos runs
            _cutoff: desired precission
            _reortho: reorthogonalization of krylov basis vectors
        num_exc: number of eigenvalues
        get_exc: get the states to the corresponding eigenvalues explicit
    """

    def __init__(
        self,
        lanczos_min=10,
        lanczos_max=50,
        lanczos_cutoff=np.finfo(float).eps * 10,
        lanczos_reortho=False,
        num_vec=1,
        get_vec=False,
        which="ARPACK",
    ):
        self.which = which  # which eigensolver
        self.lanczos_min = lanczos_min  # minimal lanczos runs
        self.lanczos_max = lanczos_max  # maximal lanczos runs
        self.lanczos_cutoff = lanczos_cutoff  # cutoff for lanczos calculation
        self.lanczos_reortho = (
            lanczos_reortho  # set reorthogonalization during lanczos run
        )
        self.num_vec = num_vec  # number of excitations
        self.get_vec = get_vec  # get states of excitations
        self.num_conv = 0  # number of steps for convergence

    def _apply_eta_to_theta(self, L0, W1, W2, R3, theta):
        """Applying the projected two-site hamiltonian eta to the corresponing
        two-site (bond) state theta, e.g. H|psi> on site i,i+1.
        ========================================================================
        Args:
            L0: left environment on site i-1 (a0, b0, a0^')
            W1: matrix product operator on site i (b0, b1, s0^', s0)
            W2: matrix product operator on site i+1 (b1, b2, s1^', s1)
            R3: right environment on site i+1 (a2, b2, a2^')
        Returns:
            theta: contracted two-site tensor with index (a0^', s0^', s1^', s2^')
        """
        theta = np.tensordot(L0, theta, axes=(0, 0))
        theta = np.tensordot(theta, W1, axes=([0, 2], [0, 2]))
        theta = np.tensordot(theta, W2, axes=([3, 1], [0, 2]))
        theta = np.tensordot(theta, R3, axes=([1, 3], [0, 1]))
        return theta

    def _mult_bra_to_ket(self, bra, ket, sites=2):
        """Multiplying bra to ket, i.e. <bra|ket>, for a two-site segment."""
        return np.squeeze(np.tensordot(bra, ket, axes=([0, 1, 2, 3], [0, 1, 2, 3])))

    #    def _mult_bra_to_ket(self, bra, ket):
    #        """ Multiplying bra to ket, e.g. <bra|ket>, for a two-site segment and
    #            squeezes the whole thing.
    #        """
    #        return(np.squeeze(np.tensordot(bra, ket, axes = ([0,3,1,2],[0,3,1,2]))))

    def _lanczos_hep(self, L0, W1, W2, R3, v0):
        """!!!!!!HAS PROBLEMS WITH MULTIPLE EIGENVALUES!!!!!!!
        Lanczos algorithm of hermitian eigenvalue problems.
        Tensor Network implementation of
        http://www.netlib.org/utk/people/JackDongarra/etemplates/node104.html +
        https://www.cond-mat.de/events/correl11/manuscripts/koch.pdf
        Uses:
            lanczos
                _min: minimial number of lanczos runs
                _max: maximal number of lanczos runs
                _cutoff: desired precission
                _reortho: reorthogonalization of krylov basis vectors (True or
                False)
        ========================================================================
        Args:
            L0: left fix point iL
            W1: site mpo iWL
            W2: site mpo iWR
            R0: right fix point iR
            v0: initial vector
        Returns:
            E0: ground state energy
            psi0: ground state
        TODO: expand method to multisite structure and FIX THIS MULTIPLE
        EIGENVALUE ISSUE...
        """
        r = v0
        E0 = 0.0
        alpha = []
        beta = [np.linalg.norm(v0)]
        Vj = []
        j = 0
        conv = False
        while not conv:
            # test if max number of lanzcos iterations has been reached
            if j == self.lanczos_max:
                conv = True
                break
            Vj.append(r / beta[j])  # v_j = r/b_j
            r = self._apply_eta_to_theta(L0, W1, W2, R3, Vj[j])  # r = H vj
            if j > 0:
                r -= Vj[j - 1] * beta[j]  # r = r - v_{j-1} b_{j}
            alpha.append(self._mult_bra_to_ket(np.conj(Vj[j]), r))  # a_j = vj* r
            r -= Vj[j] * alpha[j]  # r = r-v_j a_j
            # full orthogonalization
            if self.lanczos_reortho and j > self.lanczos_min:
                for v in Vj[-6:]:
                    r -= v * self._mult_bra_to_ket(np.conj(v), r)
            # diagonalizing tridiagonal matrix T
            if j > 1:
                ti, si = linalg.eig_banded(
                    [beta, alpha]
                )  # pretty nice function, thx obama
                if abs(E0 - min(ti)) < np.finfo(float).eps * 10:
                    conv = True
                E0 = ti[0]  # ground state energy, doesn't change under transformation
                s0 = si[:, 0]  # ground state of T in krylov basis
            beta.append(np.linalg.norm(r))
            # test boundary for convergence
            if abs(beta[j]) < self.lanczos_cutoff:
                conv = True
            # check j for min iteration steps
            if j <= max(3, self.lanczos_min):
                conv = False
            # increase step j
            j += 1
        # obtain psi0 in the original basis
        psi0 = 0
        for v, i in zip(Vj, s0):
            psi0 += v * i

        return E0, psi0 / np.linalg.norm(psi0)

    class _ham_mixed:
        """Nested Class for the ARPACK diagonalization routine. This class
        basically overwrites the matvec function in ARPACK to a tensor network
        contraction which is numerical cheaper than the direct diagonalization
        approach. Adapted from an algorithm provided by Prof. Dr. Frank Pollmann.
        Many thanks to him.
        """

        def __init__(self, L0, W1, W2, R3, dtype=float):
            self.L0 = L0
            self.W1 = W1
            self.W2 = W2
            self.R3 = R3
            self.d = W1.shape[3]
            self.chi0 = L0.shape[0]
            self.chi2 = R3.shape[0]
            self.shape = np.array(
                [self.d**2 * self.chi0 * self.chi2, self.d**2 * self.chi0 * self.chi2]
            )
            self.dtype = dtype

        def matvec(self, theta):
            theta = np.reshape(theta, (self.chi0, self.d, self.d, self.chi2))
            theta = np.tensordot(self.L0, theta, axes=(0, 0))
            theta = np.tensordot(theta, self.W1, axes=([0, 2], [0, 2]))
            theta = np.tensordot(theta, self.W2, axes=([3, 1], [0, 2]))
            theta = np.tensordot(theta, self.R3, axes=([1, 3], [0, 1]))
            theta = np.reshape(theta, ((self.d * self.d) * (self.chi0 * self.chi2)))
            if self.dtype == float:
                return np.real(theta)
            else:
                return theta

    def _arpack(self, L0, W1, W2, R3, v0, num_vec):
        """Diagonalization routine using ARPACK. The routine eigsh
        automatically detects if the input matrix is complex or real and uses
        eigs for complex and eigsh for real input. Eigsh is casting the input
        eta to LinearOperator, whereas eigs is not casting the input eta to a
        LinearOperator hence one has to do that by hand at the input point if
        eta is complex.
        ========================================================================
        Args:
            L0: left fix point
            W1: site mpo 1
            W2: site mpo 2
            R0: right fix point
            v0: initial vector
        Returns:
        """
        eta = self._ham_mixed(L0, W1, W2, R3, W1.dtype)
        if np.issubdtype(eta.dtype, np.complexfloating):
            eta = linalg.aslinearoperator(eta)
        ei, vi = linalg.eigsh(
            eta,
            k=num_vec,
            which="SA",
            return_eigenvectors=True,
            v0=v0,
            ncv=self.lanczos_max,
            tol=self.lanczos_cutoff,
        )
        return ei, vi

    def _diag(self, L0, W1, W2, R3, v0, k=1, eigensolver=None):
        if eigensolver == "ARPACK":
            ei, vi = self._arpack(L0, W1, W2, R3, v0, k)
        elif eigensolver == "Lanczos":
            ei, vi = self._lanczos_hep(L0, W1, W2, R3, v0)
        elif eigensolver == "BandLanczos":
            ei, vi = self._bandlanczos_hep(L0, W1, W2, R3, v0)
        elif eigensolver == "Arnoldi":
            ei, vi = self._arnoldi(L0, W1, W2, R3, v0, k)
        else:
            raise Exception("No Eigensolver has been choosen. Abort.")
        return ei, vi
