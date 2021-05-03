#!/usr/bin/env python3

from keras.datasets import mnist
import matplotlib.pyplot as plt
import numpy as np

(X_train, y_train), (X_test, y_test) = mnist.load_data()
print(np.shape(X_train))

plt.subplot(2,2,1)
plt.imshow(X_train[0], cmap=plt.get_cmap('gray'))
plt.subplot(2,2,2)
plt.imshow(X_train[1], cmap=plt.get_cmap('gray'))
plt.subplot(2,2,3)
plt.imshow(X_train[2], cmap=plt.get_cmap('gray'))
plt.subplot(2,2,4)
plt.imshow(X_train[3], cmap=plt.get_cmap('gray'))

plt.show()
