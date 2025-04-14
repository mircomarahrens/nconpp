import numpy as np


def dmrg2s_gate(self, l0, V1, l1, U2, l2, L0, W1, W2, R3):
    """Quantum gate for two-site DMRG."""
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


def tebd2s_gate(self, l0, V1, l1, U2, l2, Ut):
    """Quantum gate for two-site TEBD."""
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


def tdvp2s_gate(self):
    """Quantum gate for two-site TDVP."""
    raise Exception("Not implemented.")


def wave_function_prediction(self, l0, V1, l1, U2, l2):
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
