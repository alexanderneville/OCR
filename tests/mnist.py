#!/usr/bin/env python3

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn

from pprint import pprint
from keras.datasets import mnist
import matplotlib.pyplot as plt
import numpy as np

def visualise_inputs(x_train):

    plt.subplot(2,2,1)
    plt.imshow(x_train[0], cmap=plt.get_cmap('gray'))
    plt.subplot(2,2,2)
    plt.imshow(x_train[1], cmap=plt.get_cmap('gray'))
    plt.subplot(2,2,3)
    plt.imshow(x_train[2], cmap=plt.get_cmap('gray'))
    plt.subplot(2,2,4)
    plt.imshow(x_train[3], cmap=plt.get_cmap('gray'))

    plt.show()

def calc_y_activations(y_train):

    y_activations = [[[0 for _ in range(10)]] for _ in range(60000)]

    for i in range(len(y_train)):

        y_activations[i][0][y_train[i]] = 1

    return np.array(y_activations)

def calc_x_activations(x_train):

    pass

(x_train, y_train), (x_test, y_test) = mnist.load_data() # load dataset

# x_train = calc_x_activations(x_train)
y_activations = calc_y_activations(y_train)

network = nn.Network([28*28,16,16,10])
network.train(x_train, y_activations, 0.1)
