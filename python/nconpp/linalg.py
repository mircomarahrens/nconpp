import scipy


def eigs(M, krylov_dim, eigvec=False):
    """
    Compute the eigenvalues and eigenvectors of a square matrix.
    """
    return scipy.sparse.linalg.eigen.arpack.eigs(
        M, k=krylov_dim, return_eigenvectors=eigvec
    )


def eigsh(M, k, v0, ncv, tol):
    """
    Compute the eigenvalues and eigenvectors of a symmetric matrix.
    """
    return scipy.sparse.linalg.eigen.arpack.eigsheigsh(
        M,
        k=k,
        which="SA",
        return_eigenvectors=True,
        v0=v0,
        ncv=ncv,
        tol=tol,
    )


def eig_banded(M, krylov_dim, eigvec=False):
    """
    Compute the eigenvalues and eigenvectors of a symmetric band matrix.
    """
    return scipy.linalg.eig_banded(M, k=krylov_dim, return_eigenvectors=eigvec)


def aslinearoperator(M):
    """
    Return a LinearOperator object from a matrix.
    """
    return scipy.sparse.linalg.LinearOperator(M)
