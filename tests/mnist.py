#!/usr/bin/env python3

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn
from pprint import pprint
from keras.datasets import mnist
import matplotlib.pyplot as plt
import numpy as np

from dataset_functions import *

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

def test_SGD(x_train, y_train, x_test, y_test, y_values, network: nn.Network):

    network.train(x_train, y_train, 200, 0.1)
    output = network.predict(x_test)
    evaluate(output, y_test, y_values)

def test_batch_learning(x_train, y_train, x_test, y_test, y_values, network: nn.Network):

    """supply the optional mini-batch-size arguement"""

    network.train(x_train, y_train, 200, 0.1, 200)
    output = network.predict(x_test)
    evaluate(output, y_test, y_values)

def main():

    y_values = [i for i in range(10)]
    (x_train, y_train), (x_test, y_test) = mnist.load_data() # load dataset
    y_train_activations = calc_y_activations(y_train, y_values)

    x_train = x_train/255
    x_test = x_test/255

    network = nn.Network([784, 50, 50 ,10])
    test_SGD(x_train[:2000], y_train_activations[:2000], x_test[:2000], y_test[:2000], y_values, network)
    # test_batch_learning(x_train[:2000], y_train_activations[:2000], x_test[:2000], y_test[:2000], y_values network)

if __name__ == "__main__":
    main()
