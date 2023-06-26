import numpy as np

# Do not change function signatures
#
# input:
#   X is the input matrix of size n_samples x n_features.
#   pass the parameters of the kernel function via kwargs.
# output:
#   Kernel matrix of size n_samples x n_samples 
#   K[i][j] = f(X[i], X[j]) for kernel function f()

def linear(X: np.ndarray, **kwargs) -> np.ndarray:
    assert X.ndim == 2
    Y = kwargs.get('Y')
    c = kwargs.get('c')
    if c is None:
        c = 0 
    if Y is None:
        Y = X
    if Y.ndim == 1:
        Y = Y.reshape(-1 , 1 )
        Y = Y.T
    return np.dot(X, np.transpose(Y)) + c

def polynomial(X:np.ndarray,**kwargs)-> np.ndarray:
    Y = kwargs.get('Y')
    if Y is None:
        Y = X
    c = kwargs.get('c')
    d = kwargs.get('d')
    alpha = kwargs.get('alpha')
    if alpha is None :
        alpha = 1 
    if d is None :
        d = 0 
    if c is None :
        c = 0 
    assert X.ndim == 2
    if Y.ndim == 1:
        Y = Y.reshape(-1 , 1 )
        Y = Y.T
    return (alpha*(np.dot(X ,  np.transpose(Y) ) +  c))**(d)

def rbf(X:np.ndarray,**kwargs)-> np.ndarray:
    
    assert X.ndim == 2
    Y = kwargs.get('Y')
    if Y is None:
        Y = X.copy()
    gamma = kwargs.get('gamma')
    if gamma is None :
        gamma = 0.1 
    if Y.ndim == 1:
        Y = Y.reshape(-1 , 1 )
        Y = Y.T
    N , M = (X.shape[0] , Y.shape[0])
    ans = np.zeros((N,M))
    for i in range (N) :
        for j in range (M) :
            ans[i,j] = np.exp(-gamma*(np.linalg.norm(X[i] - Y[j] )**2))
    return ans

    

def sigmoid(X:np.ndarray,**kwargs)-> np.ndarray:

    Y = kwargs.get('Y')
    if Y is None:
        Y = X
    r = kwargs.get('r')
    if r is None :
        r = 1 
    gamma = kwargs.get('gamma')
    assert kwargs.get('gamma') > 0
    return np.tanh(gamma*(np.dot(np.transpose(X) , Y) + r))


def laplacian(X:np.ndarray,**kwargs)-> np.ndarray:
    assert X.ndim == 2
    Y = kwargs.get('Y')
    if Y is None:
        Y = X.copy()
    a = kwargs.get('a')
    if a is None :
        a = 0.1
    if Y.ndim == 1:
        Y = Y.reshape(-1 , 1 )
        Y = Y.T
    N , M = (X.shape[0] , Y.shape[0])
    ans = np.zeros((N,M))
    for i in range (N) :
        for j in range (M) :
            ans[i,j] = np.exp(-a*(abs(np.sum(X[i] - Y[j] ))))
    return ans

