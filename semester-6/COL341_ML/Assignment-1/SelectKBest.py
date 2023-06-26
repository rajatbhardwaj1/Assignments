import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.linear_model import LinearRegression
from sklearn.feature_selection import SelectKBest, f_classif, chi2
from sklearn.metrics import mean_squared_error
from ridgeRegression import RidgeRegression


# importing funtions
from linearRegression import LinearReg
from ridgeRegression import RidgeRegression


def append_bias(X):
    bias = np.ones((X.shape[0], 1))
    X = np.concatenate((bias, X), axis=1)
    return X

def selectkbest(train_path , val_path , test_pat ,k):

    # train dataset
    df = pd.read_csv(train_path, header=None)
    array = df.to_numpy()
    X = array[:, 2:2050]
    Y = array[:, 1:2]

    # validation dataset
    df_val = pd.read_csv(val_path, header=None)
    array_val = df_val.to_numpy()
    X_val = array_val[:, 2:2050]
    Y_val = array_val[:, 1:2]

    alphas = [0.1, 0.01, 0.001]
    Num_iter = 100


    features = SelectKBest(k=k).fit(X, Y)
    X = features.transform(X)
    X_val = features.transform(X_val)
    X = append_bias(X)
    X_val = append_bias(X_val)


    W = np.zeros((X.shape[1], 1))

    # X_test = features.transform(X_test)

    W = LinearReg(X, Y,  X_val, Y_val,0.001 , 0.1e-10, "reltol")
