from typing import List
import numpy as np
import pandas as pd 
import qpsolvers
from kernel import *

class Trainer:
    def __init__(self,kernel,C=None,**kwargs) -> None:
        self.kernel = kernel
        self.kwargs = kwargs
        self.C=C
        self.support_vectors:List[np.ndarray] = []
        self.positive_alphas:List[int] = []
        self.support_vectors_Y =  [] 
    def binary_classifier(self , X, Y):
        
        N , M = X.shape
        ker = 0 
        if self.kernel == linear:
            ker  = self.kernel(X , Y = X  ,c = self.kwargs.get('c'))
        if self.kernel == rbf:
            ker  = self.kernel(X , Y = X  ,c = self.kwargs.get('c') , gamma = self.kwargs.get('gamma'))
        if self.kernel == polynomial:
            ker = self.kernel(X , Y = X , c =self.kwargs.get('c') , d = self.kwargs.get('d') , alpha = self.kwargs.get('alpha')    )
   
        P = np.dot(Y,Y.T)*ker
        q = np.ones(N)*-1
        A = Y.T
        b = np.zeros(1)
        if self.C is None:
            G = np.diag(np.ones(N) * -1)
            h = np.zeros(N)
        else:
            g1 = np.diag(np.ones(N) * -1)
            g2 = np.diag(np.ones(N))
            G = np.vstack((g1, g2))
            a1 = np.zeros(N)
            b1 = np.ones(N) * self.C
            h = np.hstack((a1 , b1)) 

        alpha = np.zeros(N)
       
        try:
            alpha = np.array(qpsolvers.solve_qp(P, q, G, h, A, b, solver="cvxopt"))
            alpha = np.reshape(alpha , (N, 1))
        except Exception:
            alpha = np.ones((1,1))
        alpha.reshape(-1 , 1)
        sv = np.argwhere(alpha > 1e-3  )[: ,0 ]
        sv1 = np.argwhere(alpha <  self.C)[:, 0]
        sv = np.intersect1d(sv ,sv1  )
       
        self.support_vectors = [X[i] for i in sv]
        self.support_vectors_Y = [Y[i] for i in sv]
        self.positive_alphas = [alpha[i] for i  in sv]
        if len(sv) == 0 :
            
            sv = np.argmax(alpha)
            self.support_vectors = [X[sv]]
            self.support_vectors_Y = [Y[sv] ]
            self.positive_alphas = [alpha[sv]]
        X_help = alpha*Y

        # X_help = X_help * X


        self.b = 0
        for i in range(len(self.support_vectors_Y)):
            xnxs = 0
            if self.kernel == linear:
                xnxs =  self.kernel( X , Y = self.support_vectors[i]  , c = self.kwargs.get('c') )
                xnxs = X_help*xnxs
                xnxs = np.sum(xnxs)
            if self.kernel == rbf:
                xnxs =  self.kernel( X , Y = self.support_vectors[i]  , c = self.kwargs.get('c'), gamma = self.kwargs.get('gamma') )
                xnxs = X_help*xnxs
                xnxs = np.sum(xnxs)
            if self.kernel == polynomial:
                xnxs =  self.kernel(X , Y = self.support_vectors[i]  , c =self.kwargs.get('c') , d = self.kwargs.get('d' ) , alpha = self.kwargs.get('alpha')    )
                xnxs = X_help*xnxs
                xnxs = np.sum(xnxs)
            self.b += self.support_vectors_Y[i] - xnxs
        self.b /= max(len(self.support_vectors_Y),1)


    def fit(self, train_data_path:str) -> None:
        #TODO: implement
        #store the support vectors in self.support_vectors

        df_val = pd.read_csv(train_data_path , index_col=0)
        y_ind = df_val.columns.get_loc('y')
        x_ind = df_val.shape[1]
        Y = df_val.to_numpy()[: , y_ind:y_ind+1]
        x = list(range(x_ind)) 
        x.remove(y_ind)
        X = df_val.to_numpy()[: ,x]

        neging = np.argwhere(Y == 0)
        for i in neging:
            Y[i] = -1
        self.binary_classifier(X, Y)


    def binary_pred(self , X_test ):
        alphas = np.array(self.positive_alphas)
        X = np.array(self.support_vectors)
        Y = np.array(self.support_vectors_Y)
        X_help = alphas*Y
        # X_help = X_help*(X)        
        temp = 0 
        if self.kernel == linear:
            temp =np.array([((np.sum(X_help*self.kernel(X , Y = X_test[i], c = self.kwargs.get('c'))) + self.b ))for i in range(X_test.shape[0])])
        if self.kernel == rbf:
            temp =np.array([((np.sum(X_help*self.kernel(X , Y = X_test[i], c = self.kwargs.get('c'), gamma = self.kwargs.get('gamma'))) + self.b ))for i in range(X_test.shape[0])])
        if self.kernel == polynomial:
            temp =np.array([((np.sum(X_help*self.kernel(X , Y = X_test[i] , c =self.kwargs.get('c') , d = self.kwargs.get('d' ) , alpha = self.kwargs.get('alpha')    )) + self.b ))for i in range(X_test.shape[0])])

        return temp 
    

    def predict(self, test_data_path:str)->np.ndarray:
        if(len(self.support_vectors) == 0 ):
            return
        df_val = pd.read_csv(test_data_path , index_col=0)
        X_test = df_val.to_numpy()
        Y_test = np.array([])
        if 'y' in df_val:
            y_ind = df_val.columns.get_loc('y')
            x_ind = df_val.shape[1]
            Y_test = df_val.to_numpy()[: , y_ind:y_ind+1]
            x = list(range(x_ind)) 
            x.remove(y_ind)
            X_test = df_val.to_numpy()[: ,x]
        n_sam , n_feat = X_test.shape
        temp = self.binary_pred(X_test )

        temp = np.sign(temp)
        zeros  = np.argwhere(temp < 0)
        for i in zeros:
            temp[i] = 0
        if Y_test.shape[0] > 0:
            zeros_pred = np.argwhere(temp  == 0 ) 
            ones_pred = np.argwhere(temp == 1 )
            zeros_act =  np.argwhere(Y_test == 0)
            ones_act =  np.argwhere(Y_test == 1 )
            zz = np.intersect1d(zeros_pred , zeros_act)
            zo = np.intersect1d(ones_pred , zeros_act)
            oz = np.intersect1d(zeros_pred , ones_act)
            oo = np.intersect1d(ones_pred , ones_act)
         

            not_equal = np.argwhere(Y_test != temp)
            print(((n_sam - len(not_equal))/n_sam)*100 , '%')
        return temp.flatten()




