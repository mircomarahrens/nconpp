import numpy as np
import python.nconpp.linalg as linalg


def rotate_right(s: list, M: np.ndarray):
    """Rotates a singular value vector from the left to the right of an MPS
    matrix. See arxiv:0804.2509 (McCulloch's iDMRG paper) for details.

    Args:
        s (list): Schmidt values
        M (np.ndarray): MPS matrix

    Pseudocode:
        1. sM = s M
        2. U S V = svd(sM)
        3. Mt = U, sR = SV

    Returns:
        MPS Matrix Mt
        Matrix sR
    """
    # construct sM
    sM = np.tensordot(np.diag(s), M, axes=(1, 0))
    sM = np.reshape(sM, (sM.shape[0] * sM.shape[1], sM.shape[2]))
    # rotate s to the right via SVD to obtain Mt sR
    U, S, V = np.linalg.svd(sM, full_matrices=False)
    # construct Mt
    Mt = np.reshape(U, (M.shape[0], M.shape[1], U.shape[1]))
    # construct sR
    sR = np.tensordot(np.diag(S), V, axes=(1, 0))
    return Mt, sR


def rotate_left(M, s):
    """Rotates a singular value vector from the right to the left of an MPS
    matrix. See arxiv:0804.2509 (McCulloch's iDMRG paper) for details.

    Args:
        s (list): Schmidt values
        M (np.ndarray): MPS matrix

    Pseudocode:
        1. Ms = M s
        2. U S V = svd(Ms)
        3. sL = US, Mt = V

    Returns:
        sL: Matrix
        Mt: MPS Matrix
    """
    # construct Ms
    Ms = np.tensordot(M, np.diag(s), axes=(2, 0))
    Ms = np.reshape(Ms, (Ms.shape[0], Ms.shape[1] * Ms.shape[2]))
    # rotate s to the right via SVD to obtain Mt sL
    U, S, V = np.linalg.svd(Ms, full_matrices=False)
    # construct Mt
    Mt = np.reshape(V, (V.shape[0], M.shape[0], M.shape[1]))
    # construct sL
    sL = np.tensordot(U, np.diag(S), axes=(1, 0))
    return sL, Mt


def contract_left():
    raise Exception("Not implemented.")


def contract_right():
    raise Exception("Not implemented.")


def transfer_matrix(M1, M2):
    """Returns the transfer matrix on a specific site with matrices M1 and M2.
    Usually M2 is the complex conjugate of M1.

    Args:
        MPS Matrix M1,
        MPS Matrix M2

    Returns:
        T: transfer matrix
    """
    return np.transpose(np.tensordot(M1, M2, axes=(1, 1)), (0, 2, 1, 3))


def construct_leftblock_transfer(A1, s1, B2, s2):
    """Constructs the two site transfer matrix of a block A1 s1 B2 s2.

    Args:
        left Mps matrix A1
        singular vector s1
        right Mps matrix B2
        singular vector s2

    Pseudocode:
        1. s1 B2 s2 -> At2 sR s2 -> At2 PR
        2. TL = A1 A1* At2 At2* PR PR*

    Returns:
        TL: leftblock transfer object
    """
    # rotate l1 to the right of B2
    At2, sR = rotate_right(s1, B2)
    PR = np.tensordot(sR, np.diag(s2 ** (-1)), axes=(1, 0))
    # constructing the transfer matrix in left-orthogonal basis
    TL = transfer_matrix(A1, np.conj(A1))
    TL = np.reshape(TL, (TL.shape[0] * TL.shape[1], TL.shape[2], TL.shape[3]))
    TL = np.tensordot(TL, transfer_matrix(At2, np.conj(At2)), axes=([1, 2], [0, 1]))
    TL = np.tensordot(TL, PR, axes=(1, 0))
    TL = np.tensordot(TL, np.conj(PR), axes=(1, 0))
    TL = np.reshape(TL, (TL.shape[0], TL.shape[1] * TL.shape[2]))
    return TL


def update(self, result):
    # get indices, ...
    iWL = result[1]
    iWR = result[2]
    # ...and update the matrices and Schmidt values for the matrix product state
    self.A_list[iWL] = result[4]
    self.l_list[iWL] = result[5]
    self.B_list[iWR] = result[6]


def construct_rightblock_transfer(s0, A1, s1, B2):
    """Constructs the two site transfer matrix of a block s0 A1 s1 B2.

    Args:
        singular vector s0, left Mps Matrix A1, singular vector s1,
        right Mps Matrix B2

    Pseudocode:
        1. s0 A1 s2 -> s0 sL Bt1 -> PL Bt1
        2. TR = B2 B2* Bt1 Bt1* PL PL*

    Returns:
        TR: rightblock transfer object
    """
    # rotate l1 to the left of A1
    sL, Bt1 = rotate_left(A1, s1)
    PL = np.tensordot(np.diag(s0 ** (-1)), sL, axes=(1, 0))
    # constructing the transfer matrix in right-orthogonal basis
    TR = transfer_matrix(B2, np.conj(B2))
    TR = np.reshape(TR, (TR.shape[0], TR.shape[1], TR.shape[2] * TR.shape[3]))
    TR = np.tensordot(transfer_matrix(Bt1, np.conj(Bt1)), TR, axes=([2, 3], [0, 1]))
    TR = np.tensordot(np.conj(PL), TR, axes=(1, 1))
    TR = np.tensordot(PL, TR, axes=(1, 1))
    TR = np.reshape(TR, (TR.shape[0] * TR.shape[1], TR.shape[2]))
    TR = TR.T
    return TR


def calc_leftblock_eigvals(A_list, s_list, B_list, krylov_dim, eigvec=False):
    """Calculates the eigenvalues and corresponding eigenvectors (eigvec=True)
    of a left orthogonalized block.
    """
    L = len(s_list)
    ei_list = [0] * L
    if eigvec:
        vi_list = [0] * L
    for index in range(L):
        # indices
        i1 = index
        i2 = (index + 1) % L
        # get block A1 s1 B2 s2
        A1 = A_list[i1]
        s1 = s_list[i1]
        B2 = B_list[i2]
        s2 = s_list[i2]
        # constructing the transfer matrix in left-orthogonal basis
        TL = construct_leftblock_transfer(A1, s1, B2, s2)
        # diagonalizing transfer operator TL
        if eigvec:
            ei, vi = linalg.eigs(TL, k=krylov_dim, return_eigenvectors=eigvec)
            ei_list[i2] = ei
            vi_list[i2] = vi
        if not (eigvec):
            ei = linalg.eigs(TL, k=krylov_dim, return_eigenvectors=eigvec)
            ei_list[i2] = ei
    return ei_list, vi_list


def calc_rightblock_eigvals(A_list, s_list, B_list, krylov_dim, eigvec=False):
    """Calculates the eigenvalues and corresponding eigenvectors (eigvec=True)
    of a right orthogonalized block.
    """
    L = len(s_list)
    ei_list = [0] * L
    if eigvec:
        vi_list = [0] * L
    for index in range(L):
        # indices
        i0 = index
        i1 = (index + 1) % L
        i2 = (index + 2) % L
        # get block s0 A1 s1 B2
        s0 = s_list[i0]
        A1 = A_list[i1]
        s1 = s_list[i1]
        B2 = B_list[i2]
        # constructing the transfer matrix in right-orthogonal basis
        TR = construct_rightblock_transfer(s0, A1, s1, B2)
        # diagonalizing transfer operator TL
        if eigvec:
            ei, vi = linalg.eigs(TR, k=krylov_dim, return_eigenvectors=eigvec)
            ei_list[i0] = ei
            vi_list[i0] = vi
        if not (eigvec):
            ei, vi = linalg.eigs(TR, k=krylov_dim, return_eigenvectors=eigvec)
            ei_list[i0] = ei
    return ei_list, vi_list


def calc_norm(M_list, s_list, form="left"):
    """Calculates the norm of a MPS."""
    L = len(M_list)
    if form == "left":
        T = np.tensordot(M_list[0], np.conj(M_list[0]), axes=([0, 1], [0, 1]))
        for i in range(1, L):
            T = np.tensordot(T, M_list[i], axes=(0, 0))
            T = np.tensordot(T, np.conj(M_list[i]), axes=([0, 1], [0, 1]))
        T = np.tensordot(T, np.diag(s_list[-1]), axes=(0, 0))
        T = np.tensordot(T, np.diag(np.conj(s_list[-1])), axes=([0, 1], [1, 0]))
    if form == "right":
        T = np.tensordot(M_list[0], np.conj(M_list[0]), axes=(1, 1))
        T = np.tensordot(np.diag(s_list[-1]), T, axes=(1, 0))
        T = np.tensordot(np.diag(np.conj(s_list[-1])), T, axes=([0, 1], [0, 2]))
        for i in range(1, L - 1):
            T = np.tensordot(T, M_list[i], axes=(0, 0))
            T = np.tensordot(T, np.conj(M_list[i]), axes=([0, 1], [0, 1]))
        Te = np.tensordot(M_list[L - 1], np.conj(M_list[L - 1]), axes=([1, 2], [1, 2]))
        T = np.tensordot(T, Te, axes=([0, 1], [0, 1]))
    if form == "canonical":
        T = np.tensordot(M_list[0], np.conj(M_list[0]), axes=([0, 1], [0, 1]))
        for i in range(1, L - 1):
            T = np.tensordot(T, M_list[i], axes=(0, 0))
            T = np.tensordot(T, np.conj(M_list[i]), axes=([0, 1], [0, 1]))
        Te = np.tensordot(M_list[L - 1], np.conj(M_list[L - 1]), axes=([1, 2], [1, 2]))
        T = np.tensordot(T, Te, axes=([0, 1], [0, 1]))
    return T


def mult_bra_to_ket(M1_list, M2_list, s1_list, s2_list, form="left"):
    """Calculates the overlap between two MPS given in a specific form."""
    sites = len(M1_list)
    L = np.eye(M1_list[0].shape[0], M2_list[0].shape[0])
    R = np.eye(M1_list[-1].shape[2], M2_list[-1].shape[2])
    if form == "left":
        T = np.tensordot(M1_list[0], np.conj(M2_list[0]), axes=(1, 1))
        T = np.tensordot(L, T, axes=([0, 1], [0, 2]))
        for i in range(1, sites):
            T = np.tensordot(T, M1_list[i], axes=(0, 0))
            T = np.tensordot(T, np.conj(M2_list[i]), axes=([0, 1], [0, 1]))
        T = np.tensordot(T, np.diag(s1_list[-1]), axes=(0, 0))
        T = np.tensordot(T, np.diag(np.conj(s2_list[-1])), axes=(1, 0))
        T = np.tensordot(T, R, axes=([0, 1], [0, 1]))
    if form == "right":
        T = np.tensordot(M1_list[0], np.conj(M2_list[0]), axes=(1, 1))
        T = np.tensordot(np.diag(s1_list[-1]), T, axes=(1, 0))
        T = np.tensordot(np.diag(np.conj(s2_list[-1])), T, axes=(1, 2))
        T = np.tensordot(L, T, axes=([0, 1], [0, 1]))
        for i in range(1, sites):
            T = np.tensordot(T, M1_list[i], axes=(0, 0))
            T = np.tensordot(T, np.conj(M2_list[i]), axes=([0, 1], [0, 1]))
        T = np.tensordot(T, R, axes=([0, 1], [0, 1]))
    if form == "canonical":
        T = np.tensordot(M1_list[0], np.conj(M2_list[0]), axes=(1, 1))
        T = np.tensordot(L, T, axes=([0, 1], [0, 2]))
        for i in range(1, sites - 1):
            T = np.tensordot(T, M1_list[i], axes=(0, 0))
            T = np.tensordot(T, np.conj(M2_list[i]), axes=([0, 1], [0, 1]))
        T = np.tensordot(T, R, axes=([0, 1], [0, 1]))
    return T



# FIXME adapt functions below to A_list, l_list and B_list

# def calc_correlator_2p(A_list, l_list, B_list, O1, O2, i1, i2):
#     """Calculating two-point correlation for two operators
#     on different sites.
#     """
#     if M_list == None:
#         print(
#             "Abort. This function is only for canonical MPS (Vidal Representation).\
#         Use mps.restore_canonical_form() to transform the final mps into canonical form."
#         )
#         return None

#     correlation = np.tensordot(M_list[i1], O1, axes=(1, 1))
#     correlation = np.tensordot(correlation, np.conj(M_list[i1]), axes=([0, 2], [0, 1]))
#     for i in range(i1, i2):
#         prod = np.tensordot(M_list[i], np.conj(M_list[i]), axes=(1, 1))
#         correlation = np.tensordot(correlation, prod, axes=([0, 1], [0, 2]))
#         if i == i2 - 1:
#             correlation = np.tensordot(correlation, M_list[i], axes=(0, 0))
#             correlation = np.tensordot(correlation, O2, axes=(1, 1))
#             correlation = np.tensordot(
#                 correlation, np.conj(M_list[i]), axes=([0, 1, 2], [0, 2, 1])
#             )
#     return np.squeeze(correlation)


# def calc_correlator_3p(M_list, O1, O2, O3, i1, i2, i3, l_list=None, form="canonical"):
#     """Calculating three-point correlation three two operators
#     on different sites.
#     ========================================================================
#     Args:
#     Returns:
#     """
#     correlation = np.tensordot(M_list[i1], O1, axes=(1, 1))
#     correlation = np.tensordot(correlation, np.conj(M_list[i1]), axes=([0, 2], [0, 1]))
#     for i in range(i1, i3):
#         prod = np.tensordot(M_list[i], np.conj(M_list[i]), axes=(1, 1))
#         correlation = np.tensordot(correlation, prod, axes=([0, 1], [0, 2]))
#         if i == i2 - 1:
#             correlation = np.tensordot(correlation, M_list[i], axes=(0, 0))
#             correlation = np.tensordot(correlation, O2, axes=(1, 1))
#             correlation = np.tensordot(
#                 correlation, np.conj(M_list[i]), axes=([0, 2], [0, 1])
#             )

#         if i == i3 - 1:
#             correlation = np.tensordot(correlation, M_list[i], axes=(0, 0))
#             correlation = np.tensordot(correlation, O3, axes=(1, 1))
#             correlation = np.tensordot(
#                 correlation, np.conj(M_list[i]), axes=([0, 1, 2], [0, 2, 1])
#             )
#     return np.squeeze(correlation)


# def calc_correlator_4p(
#     M_list, O1, O2, O3, O4, i1, i2, i3, i4, l_list=None, form="canonical"
# ):
#     """Calculating four-point correlation for four operators
#     on different sites.
#     ========================================================================
#     Args:
#     Returns:
#     """
#     correlation = np.tensordot(M_list[i1], O1, axes=(1, 1))
#     correlation = np.tensordot(correlation, np.conj(M_list[i1]), axes=([0, 2], [0, 1]))
#     for i in range(i1, i4):
#         prod = np.tensordot(M_list[i], np.conj(M_list[i]), axes=(1, 1))
#         correlation = np.tensordot(correlation, prod, axes=([0, 1], [0, 2]))
#         if i == i2 - 1:
#             correlation = np.tensordot(correlation, M_list[i], axes=(0, 0))
#             correlation = np.tensordot(correlation, O2, axes=(1, 1))
#             correlation = np.tensordot(
#                 correlation, np.conj(M_list[i]), axes=([0, 2], [0, 1])
#             )
#         if i == i3 - 1:
#             correlation = np.tensordot(correlation, M_list[i], axes=(0, 0))
#             correlation = np.tensordot(correlation, O3, axes=(1, 1))
#             correlation = np.tensordot(
#                 correlation, np.conj(M_list[i]), axes=([0, 2], [0, 1])
#             )
#         if i == i4 - 1:
#             correlation = np.tensordot(correlation, M_list[i], axes=(0, 0))
#             correlation = np.tensordot(correlation, O4, axes=(1, 1))
#             correlation = np.tensordot(
#                 correlation, np.conj(M_list[i]), axes=([0, 1, 2], [0, 2, 1])
#             )
#     return np.squeeze(correlation)
