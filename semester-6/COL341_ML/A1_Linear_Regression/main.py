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
from bonus import Evaluate_bonus

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
parser.add_argument('--out_path' , default='data/' , type=str)
parser.add_argument('--type' , default='reltol', choices=["maxit" , "reltol" ],type=str)
parser.add_argument('--type2' , default='reltol', choices=[ "selectkbest" , "selectfrommodel"],type=str)
parser.add_argument('--section' , default=1 ,choices=[1,2,3,4,5 ,6],   type=int)
parser.add_argument('--alpha' , default=0.001 ,  type=float)
parser.add_argument('--split' , default=0.00 ,  type=float)
parser.add_argument('--threshold' , default=1e-10 ,  type=float)
parser.add_argument('--normalize' ,default=False ,type=bool)
args = parser.parse_args()


# train dataset
df = pd.read_csv(args.train_path, header=None)
array = df.to_numpy()
X = array[:, 2:2050]
Y = array[:, 1:2]

# validation dataset
df_val = pd.read_csv(args.val_path, header=None)
array_val = df_val.to_numpy()
X_val = array_val[:, 2:2050]
Y_val = array_val[:, 1:2]

# test dataset
df_test = pd.read_csv(args.test_path, header=None)
array_test = df_test.to_numpy()
X_test = array_test[:, 1:2049]
lambda_val = 5

if args.split != 0:
    X, X_test, Y, y_test = train_test_split(X, Y, test_size=args.split, random_state=42)

if(args.normalize == True):
    X = np.array(X, dtype=np.float64)
    X_val = np.array(X_val, dtype=np.float64)
    X_test = np.array(X_test, dtype=np.float64)
    mu_train = np.average(X , axis=0)
    mu_val = np.average(X_val , axis=0)
    mu_test = np.average(X_test , axis=0)
    sigma_train = np.std(X , axis=0)
    X = (X - mu_train )
    X = X / sigma_train
    X_val = (X_val  - mu_train)
    X_val = X_val / sigma_train
    X_test = (X_test  - mu_train)
    X_test = X_test / sigma_train
   


X = append_bias(X)
X_val = append_bias(X_val)
X_test = append_bias(X_test)


Num_iter = 100
Threshold = 1e-10

if(args.section == 1):
    LinearReg(X, Y, X_val , Y_val ,args.test_path, args.out_path , args.alpha , Threshold, args.type)
if(args.section == 2):
    RidgeRegression(X, Y, X_val , Y_val ,args.test_path, args.out_path , args.alpha , Threshold, args.type)
if(args.section == 3):
    skLearn(args.train_path , args.val_path , args.test_path)
if(args.section == 4 and args.type2 == 'selectkbest'):
    selectkbest(args.train_path , args.val_path , args.test_path , 1024)
if(args.section == 4 and args.type2 == 'selectfrommodel'):
    selectfrommodel(args.train_path , args.val_path , args.test_path)
if(args.section == 5):
    Evaluate(args.train_path ,X_val , Y_val ,  args.test_path , args.out_path)
if(args.section == 6):
    Evaluate_bonus(args.train_path ,X_val , Y_val ,  args.test_path , args.out_path)





