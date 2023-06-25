from typing import List
import numpy as np
from svm_binary import Trainer
import pandas as pd
from kernel import *


class Trainer_OVA:
    def __init__(self, kernel, C=None, n_classes = -1, **kwargs) -> None:
        self.kernel = kernel
        self.C = C
        self.n_classes = n_classes
        self.kwargs = kwargs
        self.svms = [] # List of Trainer objects [Trainer]
        self.zeros = []
    def _init_trainers(self):
        #TODO: implement
        #Initiate the svm trainers
        for _ in range(self.n_classes):
            t = Trainer(self.kernel , C = self.C , c = self.kwargs.get('c') , d = self.kwargs.get('d') ,alpha = self.kwargs.get('alpha') , gamma = self.kwargs.get('gamma') )
            self.svms.append(t)

    
    def fit(self, train_data_path:str, max_iter=None)->None:
        #TODO: implement
        #Store the trained svms in self.svms
        df_val = pd.read_csv(train_data_path , index_col=0)
        y_ind = df_val.columns.get_loc('y')
        x_ind = df_val.shape[1]
        Y = df_val.to_numpy()[: , y_ind:y_ind+1]
        x = list(range(x_ind)) 
        x.remove(y_ind)
        X = df_val.to_numpy()[: ,x]
        N, M = X.shape
        # for i in range(0,2):
        for i in range(self.n_classes):
            Y_help = Y.copy()
            ones = np.argwhere(Y == i+1)
            negones = np.argwhere(Y != i+1)
            for j,_ in ones:
                Y_help[j] = 1
            for j,_ in negones:
                Y_help[j] = -1 
            self.svms[i].binary_classifier(X , Y_help)
            


            

                
    def predict(self, test_data_path:str)->np.ndarray:
        
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


        N, M = X_test.shape
        prediction_matrix = np.zeros((N , self.n_classes))
        for i in range(self.n_classes):
            pred = self.svms[i].binary_pred(X_test)
            prediction_matrix[: , i] =  pred.flatten()
        predictions = np.argmax(prediction_matrix  , axis=1)+1
        ans = np.max(prediction_matrix  , axis=1)
        predictions = predictions.reshape(-1 , 1 )
        if Y_test.shape[0] > 0:
            # confmatr = np.zeros((self.n_classes , self.n_classes))
            # p = predictions.flatten()
            # Y1 = Y_test.flatten()
            # for i in range(len(p)):
            #     ii = p[i] - 1 
            #     jj = int(Y1[i]) - 1
            #     confmatr[ii , jj ] += 1 
            # print(confmatr)
            not_equal = len(np.argwhere(predictions != Y_test))
            acc = (N - not_equal)/N 
            acc *= 100
            print(acc , 'f%')
        return predictions.flatten()


    
class Trainer_OVO:
    def __init__(self, kernel, C=None, n_classes = -1, **kwargs) -> None:
        self.kernel = kernel
        self.C = C
        self.n_classes = n_classes
        self.kwargs = kwargs
        self.svms = [] # List of Trainer objects [Trainer]
        self.zeros = []
    def _init_trainers(self):
        #TODO: implement
        #Initiate the svm trainers
        for i in range(self.n_classes):
            for _ in range(i+1 , self.n_classes):
                t = Trainer(self.kernel , C = self.C , c = self.kwargs.get('c') , d = self.kwargs.get('d') ,alpha = self.kwargs.get('alpha') , gamma = self.kwargs.get('gamma') )
                self.svms.append(t)
        
    
    def fit(self, train_data_path:str, max_iter=None) -> None:


        df_val = pd.read_csv(train_data_path , index_col=0)
        y_ind = df_val.columns.get_loc('y')
        x_ind = df_val.shape[1]
        Y = df_val.to_numpy()[: , y_ind:y_ind+1]
        x = list(range(x_ind)) 
        x.remove(y_ind)
        X = df_val.to_numpy()[: ,x]
        
        N, M = X.shape  
        
        X1 = np.copy(X)
        Y1 = np.copy(Y)
        idx = np.argwhere(np.all(X1[..., :] == 0, axis=0))
        self.zeros = idx
        X1 = np.delete(X1, idx, axis=1)
        
        iter = 0 
        for i in range(self.n_classes):
            for j in range(i+1 , self.n_classes):
                reqi = np.argwhere(Y  == i+1)
                reqj = np.argwhere(Y == j+1)
                reqi = (reqi[: , 0]).tolist()
                reqj = (reqj[: , 0]).tolist()
                req = reqi + reqj
                req.sort()
                
                X_train = np.copy(X1)
                X_train = X_train[req , :]
                Y_train = np.copy(Y1)
                Y_train = Y_train[req, : ]
                
                for p in range(len(req)):
                    Y_train[p] = 1 if Y_train[p] == i+1 else -1
                self.svms[iter].binary_classifier(X_train , Y_train)
                # if iter == 0:
                #     self.svms[iter].binary_pred(X_train)
                iter += 1 
                        


    def predict(self, test_data_path:str) -> np.ndarray:
        #TODO: implement

        # df_val = pd.read_csv(test_data_path)
        # array_test = df_val.to_numpy()
        # X = array_test[:, 2:514]


        df_val = pd.read_csv(test_data_path , index_col=0)
        X = df_val.to_numpy()
        Y = np.array([])
        if 'y' in df_val:
            y_ind = df_val.columns.get_loc('y')
            x_ind = df_val.shape[1]
            Y = df_val.to_numpy()[: , y_ind:y_ind+1]
            x = list(range(x_ind)) 
            x.remove(y_ind)
            X = df_val.to_numpy()[: ,x]
        
        X = np.delete(X, self.zeros, axis=1)
        N, M = X.shape
        #Return the predicted labels
        lies_in = np.zeros((N , self.n_classes))
        iter = 0
        tpred  = []
        pred = self.svms[iter].binary_pred(X)

        for i in range(self.n_classes):
            for j in range(i+1 , self.n_classes):
                temp = self.svms[iter].binary_pred(X)

                temp  = temp.flatten()
                for n in range(N):
                    lies_in[n , i] += temp[n]
                    lies_in[n , j] -= temp[n]
                pred = self.svms[iter].binary_pred(X)

                pred = np.where(pred == 1, i, j)
                
                iter+= 1
        predictions = np.argmax(lies_in  , axis=1)+1

        predictions = predictions.reshape(-1 , 1 )
        if Y.shape[0] > 0 :
            # confmatr = np.zeros((self.n_classes , self.n_classes))
            # p = predictions.flatten()
            # Y1 = Y.flatten()
            # for i in range(len(p)):
            #     ii = p[i] - 1 
            #     jj = int(Y1[i]) - 1
            #     print(ii , jj)
            #     confmatr[ii , jj ] += 1 
            # print(confmatr)
            not_equal = len(np.argwhere(predictions != Y))
            acc = (N - not_equal)/N
            acc *= 100
            print(acc , '%')
        return np.array(predictions).flatten()



