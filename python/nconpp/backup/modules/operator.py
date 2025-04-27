import numpy as np


def init_vector_boson():
    #### VECTOR BOSONS ##########################################################
    #### Literature: S. Sachdev and R.N. Bhatt, PRB 41, 9323 (1990) #############
    d = 4
    vac = np.zeros((d, d), dtype=complex)
    I = np.eye(d)
    Ts = np.zeros((d, d), dtype=complex)
    Ts[0, 0] = 1.0
    Tsd = np.conjugate(Ts).T
    Tx = np.zeros((d, d), dtype=complex)
    Tx[0, 1] = 1.0
    Txd = np.conjugate(Tx).T
    Ty = np.zeros((d, d), dtype=complex)
    Ty[0, 2] = 1.0
    Tyd = np.conjugate(Ty).T
    Tz = np.zeros((d, d), dtype=complex)
    Tz[0, 3] = 1.0
    Tzd = np.conjugate(Tz).T
    ##############################################################################
    return (vac, I, Ts, Tx, Ty, Tz)


def init_pauli():
    """Init local site operators: spin 1/2 -> pauli matrices."""
    I = np.eye(2)
    X = np.array([[0.0, 1.0], [1.0, 0.0]])
    Y = np.array([[0.0, -1j], [1j, 0.0]])
    Z = np.array([[1.0, 0.0], [0.0, -1.0]])
    return (I, X, Y, Z)
