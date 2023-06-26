import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# train dataset
df = pd.read_csv("data/train.csv", header=None)
array = df.to_numpy()
X = array[:, 2:2050]
Y = array[:, 1:2]

# validation dataset
df_val = pd.read_csv("data/validation.csv", header=None)
array_val = df_val.to_numpy()
X_val = array_val[:, 2:2050]
Y_val = array_val[:, 1:2]

alphas = [0.1, 0.01, 0.001]
Num_iter = 100

W = np.zeros((X.shape[1], 9 ))

def append_bias(X):
    bias = np.ones((X.shape[0] , 1))
    X = np.concatenate((bias , X), axis= 1)
    return X


def hTheta_r(x, theta, denom):
    return np.exp(np.dot(np.transpose(theta),  x)[0, 0]) / denom


def loss_n(x, y, Theta):
    denom = 1
    for r in range(9):
        theta = Theta[: , r : r+1]
        denom += np.exp(np.dot(np.transpose(theta), x)[0,0])
    total = 0
    
    
    theta = Theta[:, y[0] - 1: y[0]]    

    
    return -np.log(hTheta_r(x , theta  , denom))
    


def loss(X , Y , Theta):
    N = X.shape[0]
    return (1/N)*sum(loss_n(np.transpose(X[i:i+1,:] ), Y[i] , Theta) for i in range(N))
        

def desc_i(X, Y , Theta ,denoms , r):
    # sourcery skip: for-index-underscore, sum-comprehension
    val = 0
    N = X.shape[0]
    theta_i = Theta[: , r:r+1]
    denom = 1

    for i in range(X.shape[0]):
        x = np.transpose(X[i:i+1,:] )
        y = Y[i][0]
        x = np.transpose(X[i:i+1 , :])
        y = Y[i]
        # print(denom , denoms[i])

        val += (1/N)*hTheta_r(x, theta_i , denoms[i]  )*x
        if r == y - 1:
            val -= x/N
    return val


def grad_desc(X , Y , Theta , alpha):
    desc = np.zeros(Theta.shape)
    denoms = [] 
    for i in range(X.shape[0]):
        x = np.transpose(X[i:i+1,:] )
        denom = 1
        for r in range(9):
            theta = Theta[: , r : r+1]
            denom += np.exp(np.dot(np.transpose(theta), x)[0,0])
        denoms.append(denom)
    for r in range(9):
        desc[: , r :r + 1] =  desc[: , r :r + 1] + desc_i(X, Y , Theta ,denoms ,  r)
    return Theta - alpha*desc

def Classification(X , Y , X_val , Y_val , alpha):
    print("Training data using logistic regression")
    N = X.shape[0]
    MSE_train = []
    MSE_val = []
    ITER = []
    Threshold = 0.01
    X = append_bias(X)
    Theta = np.zeros((X.shape[1], 9 ))
    Num = 110
    for iter in range(110) :
        Theta = grad_desc(X , Y , Theta ,alpha)
        mse_train = loss(X, Y, Theta)
        mse_val = loss(X_val , Y_val , Theta)
        MSE_train.append(mse_train)
        MSE_val.append(mse_val)
        ITER.append(iter)
        if(iter % 25 == 0 ):
            print("Number of iterations : " , iter , "/" , Num, "\nCurrent train loss: " , mse_train , "\ncurrent validation loss: ", mse_val )

    # plt.plot(ITER, MSE_Validation , MSE_Training)
    plt.plot(ITER , MSE_val , label='Validation Log loss')
    plt.plot(ITER , MSE_train , label='Training Log loss')
    plt.xlabel('Iterations')
    plt.ylabel('log loss')
    plt.legend()
    plt.title('Classification' )
    # plt.show()
    return Theta 

def classify(X , Theta):
    X = append_bias(X)
    prob_class = np.dot(X, Theta)
    # print(prob_class)
    
    return np.transpose([np.argmax(prob_class , axis = 1) ]) + 1

def Evaluate_bonus(train_path, X_val , Y_val ,  test_path,  outputfile):

    #-------training---------
    df_train = pd.read_csv(train_path, header=None)
    array_train = df_train.to_numpy()
    samples_train = array_train[: , 0:1]
    X_train = array_train[:, 2:2050]
    Y_train = array[:, 1:2]

    Theta = Classification(X_train  , Y_train , X_val, Y_val , 0.012)

    #-------testing----------

    df_test = pd.read_csv(test_path, header=None)
    array_test = df_test.to_numpy()
    samples_test = array_test[: , 0:1]
    X_test = array_test[:, 1:2049]
    classes = classify(X_test , Theta)
    print(samples_test.shape , classes.shape)
    output = np.concatenate((samples_test, classes) , axis = 1 )
    # print(output)
    op = pd.DataFrame(output)
    op.to_csv(outputfile + '/bonus.csv' , index = False , header= False) 



# Classification(X, Y, 0.01)