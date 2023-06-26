import numpy as np 
import pandas as pd
import matplotlib.pyplot as plt
import sklearn 
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, mean_absolute_error

def find_final_MSE(Y, Y_cap):
    return (np.average(np.square(Y_cap - Y) ))


def find_final_MAE( Y, Y_cap):
    return (np.average(np.abs(Y_cap - Y)  ))

def skLearn(train_path , val_path , test_path ):
    df = pd.read_csv(train_path, header=None)
    array = df.to_numpy()
    X = array[:, 2:2050]
    Y = array[:, 1:2]
    model = LinearRegression()
    model.fit(X, Y)
    df_validation = pd.read_csv(val_path, header=None)
    array_validation = df_validation.to_numpy()

    X_validation = array_validation[: , 2:2050]
    Y_validation = array_validation[:, 1:2]
    Y_pred_val = model.predict(X_validation)
    Y_pred_train = model.predict(X)

    MSE_val = find_final_MSE(Y_validation , Y_pred_val )
    MAE_val = find_final_MAE(Y_validation , Y_pred_val )

    MSE_train = find_final_MSE(Y , Y_pred_train )
    MAE_train = find_final_MAE(Y , Y_pred_train )

    print("The MSE on the validatioin data is: " , MSE_val)
    print("The MAE on the validation data is: " , MAE_val)
    print("The MSE on the train data is: " , MSE_train)
    print("The MAE on the train data is: " , MAE_train)


