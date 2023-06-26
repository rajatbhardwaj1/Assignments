#importing libraries
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.linear_model import RidgeClassifier
from sklearn.feature_selection import SelectFromModel
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split
import argparse

#importing funtions
from linearRegression import LinearReg
from ridgeRegression import RidgeRegression
from Classification import Evaluate
from sklearnLinReg import skLearn
from SelectKBest import selectkbest
from SelectFromModel import selectfrommodel


def append_bias(X):
    bias = np.ones((X.shape[0] , 1))
    X = np.concatenate((bias , X), axis= 1)
    return X




alphas = [0.1, 0.01, 0.001]
Num_iter = 100



# arguments
parser = argparse.ArgumentParser()
parser.add_argument('--train_path' , default='data/train.csv' , type=str )
parser.add_argument('--val_path' , default='data/validation.csv' , type=str)
parser.add_argument('--test_path' , default='data/test.csv' , type=str)
parser.add_argument('--out_path' , default='data/output.csv' , type=str)
parser.add_argument('--type' , default='maxit', choices=["maxit" , "reltol" ],type=str)
parser.add_argument('--type2' , default='maxit', choices=[ "selectkbest" , "selectfrommodel"],type=str)
parser.add_argument('--section' , default=1 ,choices=[1,2,3,4,5],   type=int)
parser.add_argument('--alpha' , default=0.001 ,  type=float)
parser.add_argument('--split' , default=0.00 ,  type=float)
parser.add_argument('--threshold' , default=1e-10 ,  type=float)
parser.add_argument('--normalize' ,default=False ,type=bool)
args = parser.parse_args()


# for i in [2 , 5,  10 , 100]:
#     # train dataset
#     df = pd.read_csv(f'data/{i}_d_train.csv', header=None)
#     array = df.to_numpy()
#     X = array[:, 0:i]
#     Y = array[:, i:i+1]

#     # validation dataset
#     df_val = pd.read_csv(f'data/{i}_d_test.csv', header=None)
#     array_val = df_val.to_numpy()
#     X_val = array_val[:, 0:i]
#     Y_val = array_val[:, i:i+1]
#     W1 = LinearReg(X, Y, X_val , Y_val   , args.alpha , 1e-5, 'maxit')


D = [2 , 5,  10 , 100]
MSE_out = np.array([1.0648322362047977 ,0.8304156016958436, 0.8535924597983259 , 1.8631697537360226])
MSE_in = np.array([0.831175145148053 , 0.9876834181992598 ,1.2076271240963106 ,0.43340146130645363 ])

Eo_Ein = abs(MSE_out - MSE_in)
print(Eo_Ein)


plt.plot(D , Eo_Ein , label='|E_out - E_in|')
plt.xlabel('Dimensions')
plt.ylabel('|E_out - E_in|')
plt.legend()
# plt.title(f'Ridge Regression with {type}' )
plt.show()
