import cv2
import numpy as np
import os
from sklearn.metrics import accuracy_score
import argparse

parser = argparse.ArgumentParser()
import pandas as pd
parser.add_argument('--train_path', type=str, required=True)
parser.add_argument('--test_path', type=str, required=True)
parser.add_argument('--out_path', type=str, required=True)
args = parser.parse_args()



def imagetovec(path_to_img):
    img = cv2.imread(path_to_img)
    resized_img = cv2.resize(img, (32, 32), interpolation=cv2.INTER_AREA)
    return resized_img.reshape((32 * 32 * 3))


def extract_images(dir_path):
    X = np.array([])
    for path in os.listdir(dir_path):
        if path[-3:] == "png":
            x = imagetovec(f"{dir_path}/{path}")
            if X.shape[0] == 0:
                X = x.reshape(-1, x.shape[0])
            else:
                X = np.concatenate(
                    (X, x.reshape(-1, x.shape[0])), axis=0
                ) 
    return X

def extract_images_with_name(dir_path):
    X = np.array([])
    img = []
    for path in os.listdir(dir_path):
        if path[-3:] == "png":
            img.append(path)
            x = imagetovec(f"{dir_path}/{path}")
            if X.shape[0] == 0:
                X = x.reshape(-1, x.shape[0])
            else:
                X = np.concatenate(
                    (X, x.reshape(-1, x.shape[0])), axis=0
                ) 
    return X,np.array(img)


TRAIN_PATH = args.train_path
TEST_PATH = args.test_path
OUT_PATH = args.out_path


dir_path = f"{TRAIN_PATH}/person"
person = extract_images(dir_path)
dir_path = f"{TRAIN_PATH}/dog"
dog = extract_images(dir_path)   
dir_path = f"{TRAIN_PATH}/airplane"
airplane = extract_images(dir_path)
dir_path = f"{TRAIN_PATH}/car"
car = extract_images(dir_path)
dir_path = TEST_PATH
Xtest,imglist = extract_images_with_name(dir_path)
Y = np.hstack((np.ones(person.shape[0]) , np.ones(dog.shape[0]) + 2, np.ones(airplane.shape[0]) + 1 ,np.zeros(car.shape[0]) ))
X = np.concatenate((person, dog , airplane,car))     


#------------------------------------ PART A ------------------------------------

from sklearn.tree import DecisionTreeClassifier

clf = DecisionTreeClassifier(max_depth=10, min_samples_split=7 , criterion='entropy')
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_32a.csv", index=False,header=False)


#------------------------------------ PART B ------------------------------------

from sklearn.feature_selection import SelectKBest
Y = np.ravel(Y)
features = SelectKBest(k=10).fit(X, Y)
Xfeat = features.transform(X)
Xtestfeat = features.transform(Xtest)
clf = DecisionTreeClassifier(max_depth=5, min_samples_split=2 , criterion='entropy')
clf = clf.fit(Xfeat,Y)
testpred = clf.predict(Xtestfeat)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_32b.csv", index=False,header=False)


#------------------------------------ PART C ------------------------------------

clf = DecisionTreeClassifier(random_state=0, ccp_alpha=0.0016969696969696972 , criterion="entropy")
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_32c.csv", index=False,header=False)


#------------------------------------ PART D ------------------------------------

from sklearn.ensemble import RandomForestClassifier
clf = RandomForestClassifier(random_state=0 , criterion='entropy',max_depth=None,min_samples_split=10 , n_estimators=200)
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_32d.csv", index=False,header=False)


#------------------------------------ PART E ------------------------------------
from xgboost import XGBClassifier
clf = XGBClassifier(max_depth = 9, n_estimators =50, subsample=0.6)
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_32e.csv", index=False,header=False)

#------------------------------------ PART H ------------------------------------

from sklearn.ensemble import RandomForestClassifier
clf = RandomForestClassifier(random_state=0 , criterion='entropy',max_depth=None,min_samples_split=10 , n_estimators=200)
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_32h.csv", index=False,header=False)
