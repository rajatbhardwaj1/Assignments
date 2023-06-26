import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def find_MSE(X, Y, W, N, l):
    return (1 / (2 * N)) * (
        np.sum(np.square(Y - np.dot(X, W))) + l * np.square(np.linalg.norm(W))
    )


def find_grad(X, Y, W, N, l):
    return -(1 / N) * (
        np.sum(np.dot((np.transpose(Y - np.dot(X, W))), X), axis=0).reshape(
            (X.shape[1], 1)
        )
        + l * W
    )


def update_weight(W, gd, alpha):
    return W - alpha * gd


def find_final_MSE(X, W, Y, N):
    return (1 / N) * (np.sum(np.square(np.dot(X, W) - Y)))


def find_final_MAE(X, W, Y, N):
    return (1 / N) * (np.sum(np.abs(np.dot(X, W) - Y)))


def append_bias(X):
    bias = np.ones((X.shape[0] , 1))
    X = np.concatenate((bias , X), axis= 1)
    return X

def RidgeRegression(X , Y , X_val , Y_val ,test_path ,out_path,   alpha , Threshold , type):
    # sourcery skip: avoid-builtin-shadow
    
    lambda_val = 5
    
    Num_iter = 100

    W = np.zeros((X.shape[1] , 1))
    print("Training data using logistic regression")
    N = X.shape[0]
    print(X.shape[0])
    MSE_Validation = []
    MSE_Training = [] 
    ITER = []
    iter = 1
     
    mse_validation_prev = 1000000000
    mse_validation = find_final_MSE(X_val, W, Y_val, X_val.shape[0])
    mse_training = find_final_MSE(X , W , Y , X.shape[0])
    while ( iter <= 1000 and type == "maxit") or (mse_validation_prev - mse_validation  > Threshold and type == "reltol"):
        iter = 1 + iter
        gd = find_grad(X, Y, W, N, lambda_val)
        W = update_weight(W, gd, alpha)
        mse_validation_prev = mse_validation
        mse_validation = find_final_MSE(X_val, W, Y_val,  X_val.shape[0])
        mse_training = find_final_MSE(X, W, Y, N)
        if(iter != 0 and iter % 10 == 0):
            print("number of iterations: " , iter , "\nCurrent MSE on Validation: " , mse_validation, "\nPrevious MSE on validation set: " , mse_validation_prev,"\nCurrent MSE on triaining: " , mse_training, "\nDifference: " , mse_validation_prev - mse_validation  )
            MSE_Training.append(mse_training)
            MSE_Validation.append(mse_validation)
            ITER.append(iter)




    print("\n------Training Finished--------\n")
    print(
        "The final MAE on train set is: ",
        find_final_MAE(X, W, Y, X.shape[0]),
    )
    print(
        "The final MSE on train set is: ",
        find_final_MSE(X, W, Y, X.shape[0]),
    )
    print(
        "The final MAE on validation set is: ",
        find_final_MAE(X_val, W, Y_val, X_val.shape[0]),
    )
    print(
        "The final MSE on validation set is: ",
        find_final_MSE(X_val, W, Y_val, X_val.shape[0]),
    )
    
    # plt.plot(ITER, MSE_Validation , MSE_Training)
    # plt.plot(ITER , MSE_Validation , label='Validation MSE')
    # plt.plot(ITER , MSE_Training , label='Training MSE')
    # plt.xlabel('Iterations')
    # plt.ylabel('Mean Squared Error(MSE)')
    # plt.legend()
    # plt.title(f'Basic Implementation with {type}' )
    # plt.show()


    # test dataset
    df_test = pd.read_csv(test_path, header=None)
    array_test = df_test.to_numpy()
    X_test = array_test[:, 1:2049]
    X_test = append_bias(X_test)
    samples = array_test[: , 0:1]
    lambda_val = 5
    Y_test = np.dot(X_test , W) 
    output = np.concatenate((samples , Y_test) , axis = 1)
    op = pd.DataFrame(output)
    op.to_csv(out_path+'/RidgeReg.csv' , index=False , header=False)
    return W 