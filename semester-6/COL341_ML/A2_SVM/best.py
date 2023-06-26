from typing import Union
from svm_binary import Trainer as T
from svm_multiclass import Trainer_OVO as T_ovo, Trainer_OVA as T_ova
from kernel import *

def best_classifier_two_class() -> T:
    """Return the best classifier for the two-class classification problem."""
    return T(rbf ,C =  10, c = 0 , d = 2 ,alpha =0.1  , gamma = 0.00001)

def best_classifier_multi_class()->Union[T_ovo,T_ova]:
    """Return the best classifier for the multi-class classification problem."""
    #TODO: implement, use best performing model with optimum values for C, kernel functions and all the parameters of the kernel functions.
    # Set Hyper-params
    # Set the trainer to either of T_ovo or T_ova
    # Create trainer with hyper-parameters
    
    return T_ovo(rbf , C = 40, n_classes=10, c = 0 , d = 2 ,alpha =0.001  , gamma = 0.01)


