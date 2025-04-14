import numpy as np
import python.nconpp.linalg as linalg

import python.nconpp.operations as operations


def entanglement_entropy(s_list):
    """Calculates the von Neumann entanglement entropy on every bond.

    Args:
        s_list (list): list of Schmidt values

    Returns:
        S (list): entanglement entropy
    """
    S = []
    for s_vec in s_list:
        S.append(-1.0 * np.inner(np.log(s_vec), s_vec))
    return S


def correlation_length(M_list, s_list, form="left", sites=2, ncv=40):
    """Construct transfer matrix over several sites, diagonalizing it
    and return correlation length.
        ========================================================================
        Input:
        Algorithm:
        Output:
        ========================================================================
        Name              | Value            | Description
        ------------------+------------------+----------------------------------
        form              | "left"           | M_list is espected to be in
                          |                  | left canonical form aka A_list.
                          | "right"          | M_list is espected to be in
                          |                  | right canonical form aka B_list.
                          | "canonical"      | M_list is espected to be in
                          |                  | canonical form (Vidal).
        ========================================================================
    """

    # constructing the transfer matrix over several sites
    T = operations.transfer_matrix(M_list[0], np.conj(M_list[0]))
    T = np.reshape(T, (T.shape[0] * T.shape[1], T.shape[2], T.shape[3]))

    for i in range(1, sites):
        T = np.tensordot(
            T,
            operations.transfer_matrix(M_list[i], np.conj(M_list[i])),
            axes=([1, 2], [0, 1]),
        )
    T = np.reshape(T, (T.shape[0], T.shape[1] * T.shape[2]))

    # Obtain the 2nd largest eigenvalue
    eta = linalg.eigs(T, k=2, which="LM", return_eigenvectors=False, ncv=ncv)

    return -sites / np.log(np.min(np.abs(eta)))


def site_expectation(M_list, operator, s_list=None, form="left"):
    """Expectation value for a site operator.
    ========================================================================
    Args:
        M_list: matrix product state
        operator: site operator [O_i]_{si si'}
    Returns:
        Ex_list: list of expectation value per site
    """
    Ex_list = []

    if form == "left":
        for i in range(len(M_list)):
            ex = np.tensordot(M_list[i], operator, axes=(1, 1))
            ex = np.tensordot(ex, np.diag(s_list[i]), axes=(1, 0))
            ex = np.tensordot(ex, np.conj(M_list[i]), axes=([0, 1], [0, 1]))
            ex = np.tensordot(ex, np.diag(np.conj(s_list[i])), axes=([0, 1], [1, 0]))
            Ex_list.append(np.squeeze(ex))

    if form == "right":
        for i in range(len(M_list)):
            ex = np.tensordot(M_list[i], operator, axes=(1, 1))
            ex = np.tensordot(ex, np.diag(s_list[i - 1]), axes=(0, 1))
            ex = np.tensordot(ex, np.conj(M_list[i]), axes=([1, 0], [1, 2]))
            ex = np.tensordot(
                ex, np.diag(np.conj(s_list[i - 1])), axes=([0, 1], [1, 0])
            )
            Ex_list.append(np.squeeze(ex))

    if form == "canonical":
        for i in range(len(M_list)):
            ex = np.tensordot(M_list[i], operator, axes=(1, 1))
            ex = np.tensordot(ex, np.conj(M_list[i]), axes=([0, 1, 2], [0, 2, 1]))
            Ex_list.append(np.squeeze(ex))

    return Ex_list
