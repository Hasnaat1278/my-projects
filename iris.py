import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn import datasets
from sklearn.model_selection import train_test_split

#Load data sets
X, y = datasets.load_iris(return_X_y=True, as_frame = True)
print(datasets.load_iris()["DESCR"])

#Features
print(X.describe())

#Datatypes
print(X.info())

#Values the outputs can take
print(y.value_counts())
y.name = "target"

#axis = 1 concatenates columns
all_variables = pd.concat([X, y], axis=1)

#Check correlations
print(all_variables.head())
print(all_variables.corr())

#Petal length and width have high correlations to the colors
#Visuals
all_variables.plot.scatter(x="sepal length (cm)", y="petal length (cm)", c="target", colormap="viridis")
plt.show()

#Regression
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.33, random_state=42, stratify=y)
print(X_train.shape)

#Checking shape of data
print(X_test.shape)

#Importing regression classifier
from sklearn.linear_model import LogisticRegression
log_reg = LogisticRegression()

#Logistic regression onto training data
print(log_reg.fit(X_train, y_train))

#Prediciting class from the previous data
new_flower = [[6.5, 3.0, 5.2, 2.0]]
print(log_reg.predict(new_flower))

#Predict probability of each class occuring
print(log_reg.predict_proba(new_flower))

#Prediction labels
y_pred = log_reg.predict(X_test)

#Finding the accuracy manually
accuracy = sum(y_pred == y_test) / len(y_test)
print(accuracy)

#Auto accuracy
from sklearn.metrics import accuracy_score
auto_accuracy = accuracy_score(y_test, y_pred)
print(auto_accuracy)