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

def calc_output(activations) -> int:

    """return predicted value from list of activations."""
 
    index: int = 0
    activations = activations.flatten()
    for i in range(len(activations)):
        if activations[i] >= activations[index]:
            index = i

    return index

def test_SGD(x_train, y_train, x_test, y_test, network: nn.Network):

    network.train(x_train, y_train, 200, 0.1)
    output = network.predict(x_test)
    evaluate(output, y_test)

def test_batch_learning(x_train, y_train, x_test, y_test, network: nn.Network):

    network.train(x_train, y_train, 200, 0.1, 200)
    output = network.predict(x_test)
    evaluate(output, y_test)

def evaluate(predictions, labels: list[int]) -> None:

    """Calculate predictions and measure accuracy."""

    total = 0

    for label, layer_activations in zip(labels, predictions):

        value = calc_output(layer_activations)
        message = "pass" if value == label else "fail"
        if value == label: total += 1
        print(f"Correct: {label}, Predicted: {value}, {message}")

    percentage = (total / len(predictions)) * 100
    print(f"\nAccuracy: {percentage}%\n")

def main():

    (x_train, y_train), (x_test, y_test) = mnist.load_data() # load dataset
    y_train_activations = calc_y_activations(y_train)

    #activations between 0 and 1 makes training more predictable
    x_train = x_train/255
    x_test = x_test/255

    # instantiate a network
    network = nn.Network([784, 50, 50 ,10])
    # test_SGD(x_train[:2000], y_train_activations[:2000], x_test[:2000], y_test[:2000], network)
    test_batch_learning(x_train[:2000], y_train_activations[:2000], x_test[:2000], y_test[:2000], network)


if __name__ == "__main__":
    main()
