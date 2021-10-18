#!/usr/bin/env python3

from dataset_functions import *
import os, sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.realpath(__file__)))+"/src")
import neural_network as nn
from keras.datasets import mnist
import matplotlib.pyplot as plt
import numpy as np


def invert_mnist_colours(samples):
    threshold = np.full((28,28), 255.0)
    for i in range(len(samples)):
        samples[i] = np.subtract(threshold, samples[i])


def display_numbers(characters):
    for i in range(len(characters)):
        plt.subplot(2,5,i+1, frame_on=False)
        plt.axis("off")
        plt.imshow(characters[i], cmap=plt.get_cmap('gray'))
    plt.show()


def test_nn_with_mnist():
    (x_train, y_train), (x_test, y_test) = mnist.load_data()
    labels = [i for i in range(10)]
    network = nn.Network([28*28, 50, 50, len(list(set(labels)))], list(set(labels)))
    y_correct = network.calc_y_activations(network.labels, y_train[:2000])
    network.train(x_train[:2000], y_correct, 1000)
    y_predicted = network.predict(x_test[:200])
    outputs = network.calc_results(network.labels, y_predicted)
    total_correct = 0
    for i in range(len(outputs)):
        if outputs[i] == y_test[i]:
            total_correct += 1
    percentage = total_correct/200
    percentage *= 100
    print(f"accuracy {percentage}%")
    network.export_layout("./models/mnist_model.json")


if __name__ == "__main__":
    test_nn_with_mnist()
