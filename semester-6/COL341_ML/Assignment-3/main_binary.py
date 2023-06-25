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


X_face = np.array([])
X_noface = np.array([])
X_test = np.array([])
dir_path = f"{TRAIN_PATH}/person"
X_face = extract_images(dir_path)
dir_path = f"{TRAIN_PATH}/dog"
X_noface_1 = extract_images(dir_path)   
dir_path = f"{TRAIN_PATH}/airplane"
X_noface_2 = extract_images(dir_path)
dir_path = f"{TRAIN_PATH}/car"
X_noface_3 = extract_images(dir_path)
dir_path = TEST_PATH
Xtest,imglist = extract_images_with_name(dir_path)

X_noface = np.concatenate((X_noface_1, X_noface_2, X_noface_3 ),axis=0)



X = np.concatenate((X_face, X_noface))     
Y = np.hstack((np.ones(X_face.shape[0]) , np.zeros(X_noface.shape[0])))
Y = Y.reshape(Y.shape[0] , 1)





#------------------------------------ PART A ------------------------------------


class Node():
    def __init__(self, feature_index = None , threshold = None , left = None , right = None ,gain = None , value = None) -> None:
        self.feature_index = feature_index
        self.threshold = threshold
        self.left = left
        self.right = right
        self.gain = gain
        self.value = value

        
class self_made_DecisionTreeClassifier:
    def __init__(self, min_samples_split=7, max_depth=10, criterion ="gini") -> None:
        self.root = None
        self.min_samples_split = min_samples_split
        self.max_depth = max_depth
        self.criterion = criterion

    def build_tree(self, X_face, X_noface, depth = 0):
        num_samples_face = X_face.shape[0]
        num_samples_noface = X_noface.shape[0]
        unique_left = set()
        unique_right = set()
        uniquethres = set()
        numfeatures = 0
        numfeatures = X_face.shape[1] if (num_samples_face > 0) else X_noface.shape[1]
        if (
            num_samples_face + num_samples_noface >= self.min_samples_split
            and depth <= self.max_depth
        ):
            bestsplit = {}
            max_info_gain = -float("inf")
            for index in range(numfeatures):
                if num_samples_face > 0:
                    unique_left = set(np.unique(X_face[:, index]))
                if num_samples_noface > 0:
                    unique_right = set(np.unique(X_noface[:, index]))
                uniquethres = unique_right.union(unique_left)
                for threshold in uniquethres:
                    mask_face = X_face[:, index] <= threshold
                    mask_noface = X_noface[:, index] <= threshold
                    Xfaceleft = np.count_nonzero(mask_face)
                    Xfaceright = num_samples_face - Xfaceleft
                    Xnofaceleft = np.count_nonzero(mask_noface)
                    Xnofaceright = num_samples_noface - Xnofaceleft
                    len_left, len_right = (
                        Xfaceleft + Xnofaceleft,
                        Xfaceright + Xnofaceright,
                    )
                    if len_left > 0 and len_right > 0:
                        cur_info_gain = self.info_gain(
                            Xfaceleft, Xnofaceleft, Xfaceright, Xnofaceright, self.criterion
                        )
                        if cur_info_gain > max_info_gain:
                            bestsplit["feature_index"] = index
                            bestsplit["threshold"] = threshold
                            bestsplit["maskface"] = mask_face
                            bestsplit["masknoface"] = mask_noface
                            bestsplit["info_gain"] = cur_info_gain
                            max_info_gain = cur_info_gain
                    
            if bestsplit["info_gain"] > 0:
                Xfaceleft , Xfaceright = np.split(X_face , [np.sum(bestsplit["maskface"])] , axis=0)
                Xnofaceleft , Xnofaceright = np.split(X_noface , [np.sum(bestsplit["masknoface"])] , axis=0)
                left_subtree = self.build_tree(Xfaceleft, Xnofaceleft , depth+1)
                right_subtree = self.build_tree(Xfaceright, Xnofaceright , depth+1)
                return Node(bestsplit["feature_index"],bestsplit["threshold"],left_subtree,right_subtree,bestsplit["info_gain"])
        val = 1 if num_samples_face >= num_samples_noface else 0 
        return Node(value=val)
        

    def fit(self, X_face , X_noface):
        self.root = self.build_tree(X_face ,X_noface)

    
    def predict(self, X):
        return [self.make_prediction(x, self.root) for x in X]
        
        
    def make_prediction(self, x, tree):
        if tree.value != None : 
            return tree.value
        curval = x[tree.feature_index]
        if curval <= tree.threshold:
            return self.make_prediction(x,tree.left)
        else :
            return self.make_prediction(x, tree.right)
    
    
    def gini_index(self,face, noface):
        total = float(face + noface)
        pface = float(face) / total
        pnoface = float(noface) / total
        return 1 - pface * pface - pnoface * pnoface
    

    def entropy(self,face, noface):
        total = face + noface
        pface = face / total
        pnoface = noface / total
        if pface < 1e-10 or pnoface < 1e-10:
            return 0
        ans = pface *np.log2(pface)  + pnoface * np.log2(pnoface) 
        ans *= -1
        return ans 
    def info_gain(self, Xfaceleft, Xnofaceleft, Xfaceright, Xnofaceright, mode):
        face = Xfaceleft + Xfaceright
        noface = Xnofaceleft + Xnofaceright
        left = Xfaceleft + Xnofaceleft
        right = Xfaceright + Xnofaceright
        total = left + right
        weightleft = left / total
        weightright = right / total
        if mode == "gini":
            return (
                self.gini_index(face, noface)
                - weightleft * self.gini_index(Xfaceleft, Xnofaceleft)
                - weightright * self.gini_index(Xfaceright, Xnofaceright)
            )
        else:
            return (
                self.entropy(face, noface)
                - weightleft * self.entropy(Xfaceleft, Xnofaceleft)
                - weightright * self.entropy(Xfaceright, Xnofaceright)

            )
classifier = self_made_DecisionTreeClassifier(criterion="entropy")
classifier.fit(X_face , X_noface )
testpred = classifier.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31a.csv", index=False,header=False)


# #--------------------------------------PART B --------------------------------------

from sklearn.tree import DecisionTreeClassifier

clf = DecisionTreeClassifier(max_depth=10, min_samples_split=7,criterion="entropy")
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31b.csv", index=False,header=False)


#--------------------------------------PART C --------------------------------------


from sklearn.feature_selection import SelectKBest
Y = np.ravel(Y)
features = SelectKBest(k=10).fit(X, Y)
X_feat = features.transform(X)
Xtest_feat = features.transform(Xtest)
clf = DecisionTreeClassifier(max_depth=None, min_samples_split=2,criterion="entropy")
clf = clf.fit(X_feat,Y)
testpred = clf.predict(Xtest_feat)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31c.csv", index=False,header=False)


#--------------------------------------PART D --------------------------------------

clf = DecisionTreeClassifier(random_state=0, ccp_alpha=0.0027625497690715057 , criterion="entropy")
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31d.csv", index=False,header=False)


#--------------------------------------PART E --------------------------------------

from sklearn.ensemble import RandomForestClassifier

clf = RandomForestClassifier(random_state=0 , criterion="entropy",max_depth=None, min_samples_split=5,n_estimators=100)
clf = clf.fit(X, Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31e.csv", index=False,header=False)


#--------------------------------------PART F --------------------------------------

from xgboost import XGBClassifier
clf = XGBClassifier(max_depth= 5, n_estimators= 30, subsample= 0.5)
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31f.csv", index=False,header=False)


#--------------------------------------PART H --------------------------------------

from xgboost import XGBClassifier
clf = XGBClassifier(max_depth= 5, n_estimators= 30, subsample= 0.5)
clf = clf.fit(X,Y)
testpred = clf.predict(Xtest)
df = pd.DataFrame({'Column 1': imglist, 'Column 2': testpred})
df.to_csv(f"{OUT_PATH}/test_31h.csv", index=False,header=False)
